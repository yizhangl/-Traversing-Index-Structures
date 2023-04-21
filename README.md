# Traversing Index Structures

This is a UBC assignment about traversing index structures.

Your goal in this assignment is to write code to traverse a file system index structure so that you can translate logical block numbers into physical block numbers (i.e., disk addresses).

You are to write a function:

    uint64_t lbnToPbn(CS313inode_t *ip, uint64_t lbn, uint64_t block_size, block_t(*getBlock)(uint64_t block_no))
    
which takes a special CS313 inode (whose definition you will find in lbnToPbn.h), a logical block number, the file system block size, and a function you should use to obtain diskblocks, and returns the disk address where the specified logical block can be found.

You will want to read lbnToPbn.h carefully as it describes the particular indexing structure that we are using for this exercise!

Your code must handle all the following conditions:

If the lbn is beyond the end of the file, then lbnToPbn should return BAD_PBN.
If the lbn represents a hole in the file (a location at which no block has been allocated) lbnToPbn should return HOLE_PBN, which is the physical block number stored in the index structure for a hole in a file.
The last parameter, getBlock is a function pointer. Call it with a physical disk address and it will return a block structure (see lbnToPbn.h) that contains the contents of the disk block with the given physical block number. If the pbn is invalid, get_block returns an invalid block.
