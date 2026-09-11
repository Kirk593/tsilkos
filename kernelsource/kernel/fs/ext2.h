#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>

typedef struct {
    uint32_t inode_count;
    uint32_t block_count;
    uint32_t reserved_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t first_data_block;
    uint32_t block_size;
    uint32_t blocks_per_group;
    uint32_t inodes_per_group;
    uint16_t magic;
} __attribute__((packed)) Ext2Superblock;

Ext2Superblock *read_superblock();

#endif