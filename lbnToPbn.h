#ifndef LBN_TO_PBN_H
#define LBN_TO_PBN_H

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#define BAD_PBN  ((uint64_t)-1)
#define HOLE_PBN ((uint64_t) 0)

#define NUM_INDIRECT 6
#define HAS_3INDIRECT 1
#define MAXIMUM_BLOCK_SIZE	8192

/*
 * This is a made up inode which uses a fixed length array of disk addresses.
 * The only fields we use for this assignment are i_size, i_indirect and i_3indirect.
 * The inode contains 6 disk addresses for Indirect blocks and
 * one disk address for a Triple-Indirect block.
 * This inode has 64-bit disk addresses.
 */

typedef struct CS313inode {
  uint64_t i_size;        // File size
  uint16_t i_mode;        // Mode (type of file and permissions)
  uint16_t i_uid;         // Owner's user ID
  uint16_t i_gid;         // Owner's group ID
  uint16_t i_links_count; // Reference counter (number of hard links)
  uint32_t i_atime;       // Last access time
  uint32_t i_ctime;       // Creation time
  uint32_t i_mtime;       // Last modification time
  uint32_t i_dtime;       // Deletion time
  uint32_t i_blocks;      // Number of 512-byte blocks reserved for this inode
  uint32_t i_flags;       // Flags
  uint64_t i_indirect[6]; // Indirect blocks
  uint64_t i_3indirect; // Triple-Indirect block
  uint32_t i_generation;  // File version (used for NFS)
  uint32_t reserved;      // Reserved for future use
} CS313inode_t;

typedef struct block {
  uint32_t is_valid;
  uint8_t data[MAXIMUM_BLOCK_SIZE]; 
} block_t;

uint64_t lbnToPbn(CS313inode_t *inode, uint64_t lbn, uint64_t block_size, block_t(*getBlock)(uint64_t block_no));

#endif
