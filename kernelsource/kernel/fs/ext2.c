#include <stdint.h>
#include "ext2.h"
#include "ata.h"

Ext2Superblock *read_superblock() {
    static uint8_t buffer[512];

    ata_read(2, buffer);

    Ext2Superblock *sb = (Ext2Superblock *)buffer;

    if (sb->magic != 0xEF53) {
        return 0;
    }

    return sb;
}