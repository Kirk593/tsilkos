#ifndef BOOT_H
#define BOOT_H

typedef enum {
    BOOT_MODE_UNKNOWN,
    BOOT_MODE_BIOS,
    BOOT_MODE_UEFI32,
    BOOT_MODE_UEFI64
} boot_mode_t;

void boot_detect(void *multiboot_info);

const char *boot_get_bootloader_name(void);
boot_mode_t boot_get_mode(void);

#endif