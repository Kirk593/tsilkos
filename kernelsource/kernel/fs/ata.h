#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_read(uint32_t lba, uint8_t *buffer);
void ata_write(uint32_t lba, uint8_t *buffer);

#endif