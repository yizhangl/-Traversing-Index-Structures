#include "catch.hpp"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

extern "C"
{
#include "lbnToPbn.h"
}

// ***** We recommend that you test with some other block sizes! *****
#define N_RANDOM_TESTS 999
#define TEST_BLOCK_SIZE 1024
#define VERY_LARGE_NUMBER 0xFFFFFFFF

static block_t valid_block = {1};

block getBlock(uint64_t block_no) {
    block_t block = valid_block;
    const uint64_t ptrsPerBlock = TEST_BLOCK_SIZE / sizeof(uint64_t);
    uint64_t *pointers = (uint64_t *)&block.data;

    // if the input is good, fill the right index with the right data
    if (block_no == 0xFF) {
	for (uint64_t i = 0; i <= ptrsPerBlock; i++) {
	    pointers[i] = 0x100 + i;
	}
    } else if (0x100 <= block_no && block_no < 0x1000000 + ptrsPerBlock * ptrsPerBlock * ptrsPerBlock) {
    for (uint64_t i = 0; i <= ptrsPerBlock; i++) {
	    pointers[i] = 0x100 * block_no + i;
	}
    } else {
	// fill the block with bad data
	for (uint64_t i = 0; i < TEST_BLOCK_SIZE / 4; i++) {
	    pointers[i] = 0xBADBADBADBAD;
	}
    }
    return block;
}

TEST_CASE("test_indirect_block_pointers", "[weight=1][part=test]")
{
    // block_size
    uint64_t block_size = TEST_BLOCK_SIZE;
    const uint64_t ptrsPerBlock = block_size / sizeof(uint64_t);

    // inode
    CS313inode_t inode;

    // Small files test
    inode.i_size = NUM_INDIRECT * ptrsPerBlock * block_size;

    for (uint64_t i = 0; i < NUM_INDIRECT; i++) {
        inode.i_indirect[i] = 0x100 + i; 
    }

    // Test each of the indirect block pointers
    // Notice how these numbers are similar to the ones in the
    // definition of getBlock
    for (int i = 0; i < NUM_INDIRECT; i += 1) {
	uint64_t lbn = i * ptrsPerBlock + i;
	uint64_t expected_pbn = 0x10000 + 0x100 * i + i;
	uint64_t actual_pbn = lbnToPbn(&inode, lbn, block_size, getBlock);
	REQUIRE(actual_pbn == expected_pbn);
    }
    REQUIRE(lbnToPbn(&inode, NUM_INDIRECT * ptrsPerBlock, block_size, NULL) == BAD_PBN);
}

TEST_CASE("test_triple_indirect_block_pointer", "[weight=1][part=test]")
{
    // block_size
    uint64_t block_size = TEST_BLOCK_SIZE;
    const uint64_t ptrsPerBlock = block_size / sizeof(uint64_t);
    // inode
    CS313inode_t inode;

    // The file is the maximum size that requires a 3-level tree
    inode.i_size = block_size * ptrsPerBlock * (NUM_INDIRECT + ptrsPerBlock * ptrsPerBlock);

    // All the indirect pointers are invalid 
    for (int i = 0; i < NUM_INDIRECT; i++) {
	inode.i_indirect[i] = 0xBADBADBADBAD;
    }
    inode.i_3indirect = 0xFF;
    
    // Test each of the triple-indirect block pointers
    // Notice how these numbers are similar to the ones in the
    // definition of getBlock
    for (int i = 0; i < ptrsPerBlock * ptrsPerBlock * ptrsPerBlock; i+= ptrsPerBlock * ptrsPerBlock + ptrsPerBlock + 1) {
	uint64_t lbn = NUM_INDIRECT * ptrsPerBlock + i;
	uint64_t l1 = (i / (ptrsPerBlock * ptrsPerBlock)) % ptrsPerBlock;
	uint64_t l2 = (i / ptrsPerBlock) % ptrsPerBlock;
	uint64_t l3 = i % ptrsPerBlock;
	uint64_t expected_pbn = 0x1000000 + l1 * 0x10000 + l2 * 0x100 + l3;
	uint64_t actual_pbn = lbnToPbn(&inode, lbn, block_size, getBlock);
        REQUIRE(actual_pbn == expected_pbn);
    }
    REQUIRE(lbnToPbn(&inode, NUM_INDIRECT * ptrsPerBlock + ptrsPerBlock * ptrsPerBlock * ptrsPerBlock, block_size, NULL) == BAD_PBN);
}

TEST_CASE("test_random_triple_indirect_block_pointer", "[weight=1][part=test]")
{
    // block_size
    uint64_t block_size = TEST_BLOCK_SIZE;
    const uint64_t ptrsPerBlock = block_size / sizeof(uint64_t);
    // inode
    CS313inode_t inode;

    // The file is the maximum size that requires a 3-level tree
    inode.i_size = block_size * ptrsPerBlock * (NUM_INDIRECT + ptrsPerBlock * ptrsPerBlock);

    // All the indirect pointers are invalid 
    for (int i = 0; i < NUM_INDIRECT; i++) {
	inode.i_indirect[i] = 0x100 + i;
    }
    inode.i_3indirect = 0xFF;

    // Test a bunch of random lbns
    for (int i = 0; i < N_RANDOM_TESTS; i++) {
	uint64_t lbn = rand() % (inode.i_size / block_size);
        uint64_t l1, l2, l3;
        if (lbn < NUM_INDIRECT * ptrsPerBlock) {
            l1 = lbn / ptrsPerBlock;
            l2 = lbn % ptrsPerBlock;
        }
        else {
	    l1 = ((lbn - NUM_INDIRECT * ptrsPerBlock) / (ptrsPerBlock * ptrsPerBlock)) % ptrsPerBlock;
	    l2 = ((lbn - NUM_INDIRECT * ptrsPerBlock) / ptrsPerBlock) % ptrsPerBlock;
            l3 = (lbn - NUM_INDIRECT * ptrsPerBlock) % ptrsPerBlock;

        }
        uint64_t expected_pbn = lbn < NUM_INDIRECT * ptrsPerBlock ? 0x10000 + l1 * 0x100 + l2 : 0x1000000 + l1 * 0x10000 + l2 * 0x100 + l3;
        uint64_t actual_pbn = lbnToPbn(&inode, lbn, block_size, getBlock);	
        REQUIRE(actual_pbn == expected_pbn);
    }
}

TEST_CASE("test_too_large", "[weight=1][part=test]"){
    // When the logical block number a caller requests is too large
    // (what is too large?), your lbnToPbn() should immediately return BAD_PBN
    // without needing to do any calculation.
    // **** Write your own test here. ****

    // block_size
    uint64_t block_size;

    // inode
    CS313inode_t inode;
    
    // getBlock
    auto getBlock = [](uint64_t block_no) {
        return valid_block;
    };

    uint64_t lbn;
    uint64_t expected_pbn;
    // uint64_t actual_pbn = lbnToPbn(&inode, lbn, block_size, getBlock);

    // REQUIRE(actual_pbn == expected_pbn);
}
