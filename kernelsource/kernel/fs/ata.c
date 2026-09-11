void ata_read(uint32_t lba, uint8_t *buffer) {
    // send command
    __asm__("outb %0, %1" : : "a"((uint8_t)0x02), "Nd"(0x1F2));
    __asm__("outb %0, %1" : : "a"((uint8_t)lba), "Nd"(0x1F3));
    __asm__("outb %0, %1" : : "a"((uint8_t)(lba >> 8)), "Nd"(0x1F4));
    __asm__("outb %0, %1" : : "a"((uint8_t)(lba >> 16)), "Nd"(0x1F5));
    __asm__("outb %0, %1" : : "a"((uint8_t)(0xE0 | (lba >> 24))), "Nd"(0x1F6));
    __asm__("outb %0, %1" : : "a"((uint8_t)0x20), "Nd"(0x1F7));

    // wait for drive to be ready
    for (int i = 0; i < 400; i++) {
        __asm__("inb %1, %0" : "=a"(buffer[0]) : "Nd"(0x1F7));
    }

    // read 256 words (512 bytes = 1 sector)
    for (int i = 0; i < 256; i++) {
        uint16_t data;
        __asm__("inw %1, %0" : "=a"(data) : "Nd"(0x1F0));
        buffer[i * 2]     = data & 0xFF;
        buffer[i * 2 + 1] = data >> 8;
    }
}
void ata_read(uint32_t lba, uint8_t *buffer) {
    // send command
    __asm__("outb %0, %1" : : "a"((uint8_t)0x02), "Nd"(0x1F2));
    __asm__("outb %0, %1" : : "a"((uint8_t)lba), "Nd"(0x1F3));
    __asm__("outb %0, %1" : : "a"((uint8_t)(lba >> 8)), "Nd"(0x1F4));
    __asm__("outb %0, %1" : : "a"((uint8_t)(lba >> 16)), "Nd"(0x1F5));
    __asm__("outb %0, %1" : : "a"((uint8_t)(0xE0 | (lba >> 24))), "Nd"(0x1F6));
    __asm__("outb %0, %1" : : "a"((uint8_t)0x20), "Nd"(0x1F7));

    // wait for drive to be ready
    for (int i = 0; i < 400; i++) {
        __asm__("inb %1, %0" : "=a"(buffer[0]) : "Nd"(0x1F7));
    }

    // read 256 words (512 bytes = 1 sector)
    for (int i = 0; i < 256; i++) {
        uint16_t data;
        __asm__("inw %1, %0" : "=a"(data) : "Nd"(0x1F0));
        buffer[i * 2]     = data & 0xFF;
        buffer[i * 2 + 1] = data >> 8;
    }
}

void ata_write(uint32_t lba, uint8_t *buffer) {
    // send command
    __asm__("outb %0, %1" : : "a"((uint8_t)0x02), "Nd"(0x1F2));
    __asm__("outb %0, %1" : : "a"((uint8_t)lba), "Nd"(0x1F3));
    __asm__("outb %0, %1" : : "a"((uint8_t)(lba >> 8)), "Nd"(0x1F4));
    __asm__("outb %0, %1" : : "a"((uint8_t)(lba >> 16)), "Nd"(0x1F5));
    __asm__("outb %0, %1" : : "a"((uint8_t)(0xE0 | (lba >> 24))), "Nd"(0x1F6));
    __asm__("outb %0, %1" : : "a"((uint8_t)0x30), "Nd"(0x1F7)); // 0x30 = write

    // write 256 words
    for (int i = 0; i < 256; i++) {
        uint16_t data = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        __asm__("outw %0, %1" : : "a"(data), "Nd"(0x1F0));
    }
}