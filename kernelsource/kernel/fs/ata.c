#include "ata.h"
#include <stdint.h>

#define ATA_DATA       0x1F0
#define ATA_SECCOUNT0  0x1F2
#define ATA_LBA0       0x1F3
#define ATA_LBA1       0x1F4
#define ATA_LBA2       0x1F5
#define ATA_HDDEVSEL   0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7
#define ATA_ALTSTATUS  0x3F6

#define ATA_CMD_READ_PIO   0x20
#define ATA_CMD_WRITE_PIO  0x30
#define ATA_CMD_CACHE_FLUSH 0xE7

#define ATA_SR_ERR 0x01
#define ATA_SR_DRQ 0x08
#define ATA_SR_DF  0x20
#define ATA_SR_BSY 0x80

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t value;
    __asm__ volatile ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void ata_io_wait(void)
{
    (void)inb(ATA_ALTSTATUS);
    (void)inb(ATA_ALTSTATUS);
    (void)inb(ATA_ALTSTATUS);
    (void)inb(ATA_ALTSTATUS);
}

static int ata_wait_drq(void)
{
    for (uint32_t timeout = 0; timeout < 1000000u; ++timeout) {
        uint8_t status = inb(ATA_STATUS);
        if (status & (ATA_SR_ERR | ATA_SR_DF))
            return 0;
        if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
            return 1;
    }
    return 0;
}

static void ata_select_lba28(uint32_t lba)
{
    outb(ATA_HDDEVSEL, (uint8_t)(0xE0u | ((lba >> 24) & 0x0Fu)));
    ata_io_wait();
    outb(ATA_SECCOUNT0, 1);
    outb(ATA_LBA0, (uint8_t)lba);
    outb(ATA_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_LBA2, (uint8_t)(lba >> 16));
}

void ata_read(uint32_t lba, uint8_t *buffer)
{
    if (!buffer || lba > 0x0FFFFFFFu)
        return;

    ata_select_lba28(lba);
    outb(ATA_COMMAND, ATA_CMD_READ_PIO);

    if (!ata_wait_drq())
        return;

    for (uint32_t i = 0; i < 256; ++i) {
        uint16_t data = inw(ATA_DATA);
        buffer[i * 2] = (uint8_t)data;
        buffer[i * 2 + 1] = (uint8_t)(data >> 8);
    }
}

void ata_write(uint32_t lba, uint8_t *buffer)
{
    if (!buffer || lba > 0x0FFFFFFFu)
        return;

    ata_select_lba28(lba);
    outb(ATA_COMMAND, ATA_CMD_WRITE_PIO);

    if (!ata_wait_drq())
        return;

    for (uint32_t i = 0; i < 256; ++i) {
        uint16_t data = (uint16_t)buffer[i * 2] |
                        (uint16_t)((uint16_t)buffer[i * 2 + 1] << 8);
        outw(ATA_DATA, data);
    }

    outb(ATA_COMMAND, ATA_CMD_CACHE_FLUSH);
    while (inb(ATA_STATUS) & ATA_SR_BSY) {
    }
}
