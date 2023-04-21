#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "lbnToPbn.h"

/**
 * A simple helper function. You do not have to use this.
 * @arg block: an indirect / double-indirect / triple-indirect block
 * @arg index: the index to access within the block
 * @returns a block pointer located at the "index"th location inside "block"
 */
static inline uint64_t get_pointer(block_t block, uint64_t index)
{
    return ((uint64_t *)&block.data)[index];
}

/**
 * @arg inode: an inode of a file
 * @arg lbn: a logical block number of the file
 * @arg block_size: the block size of the entire extended v6 file system
 * @arg getBlock: a function that takes a block number (block pointer) and returns the block itself
 * @returns the physical block number, translated from the logical block number using the inode
 *          BAD_PBN if lbn is a block beyond the end of a file
 *          HOLE_PBN if lbn is a "hole" in the file
 */
uint64_t lbnToPbn(CS313inode_t *inode, uint64_t lbn, uint64_t block_size, block_t(*getBlock)(uint64_t block_no))
{
    const uint64_t ptrsPerBlock = block_size / sizeof(uint64_t);

    if (lbn * block_size > inode->i_size) {
        return BAD_PBN;
    }


    // present in indirect blocks
    if (lbn < NUM_INDIRECT * ptrsPerBlock) {
        // uint64_t i = lbn / (1 + ptrsPerBlock); // which indirect block
        uint64_t i = lbn / ptrsPerBlock; // which indirect block

        uint64_t blockValue = inode->i_indirect[i];
        block_t block = getBlock(blockValue);

        if (blockValue == 0) {
            return HOLE_PBN;
        }

        uint64_t indexValue = lbn % ptrsPerBlock;
        uint64_t block_no = get_pointer(block, indexValue);
        
        return block_no;
    }


    int updated = lbn - NUM_INDIRECT * ptrsPerBlock;

    // present in triple indirect blocks
    // there are three different possibilities for it
    if (updated < ptrsPerBlock * ptrsPerBlock * ptrsPerBlock) {
        if (inode->i_3indirect == 0) {
            return HOLE_PBN;
        }

        uint64_t block_3 = inode->i_3indirect;
        block_t block = getBlock(block_3);

        uint64_t index = updated / (ptrsPerBlock * ptrsPerBlock);
        uint64_t block_n1 = get_pointer(block, index);

        if (block_n1 == 0) {
            return HOLE_PBN;
        }

        uint64_t index1 = (updated / ptrsPerBlock) % ptrsPerBlock;
        block_t block_n2 = getBlock(block_n1);
        uint64_t ptr = get_pointer(block_n2, index1);

        if (ptr == 0) {
            return HOLE_PBN;
        }

        uint64_t index2 = updated % ptrsPerBlock;
        uint64_t block_n3 = get_pointer(getBlock(ptr), index2);

        return block_n3;
    }

    return BAD_PBN;
}
