#include <stdint.h>
#include "boot.h"

typedef struct {
    uint32_t type;
    uint32_t size;
} mb2_tag_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    char string[];
} mb2_string_tag_t;

static const char *bootloader_name = "unknown";
static boot_mode_t boot_mode = BOOT_MODE_UNKNOWN;

void boot_detect(void *multiboot_info)
{
    uint8_t *ptr = (uint8_t *)multiboot_info + 8;

    boot_mode = BOOT_MODE_UNKNOWN;
    bootloader_name = "unknown";

    for (;;) {
        mb2_tag_t *tag = (mb2_tag_t *)ptr;

        /* Multiboot2 end tag */
        if (tag->type == 0)
            break;

        /* Bootloader name */
        if (tag->type == 2) {
            mb2_string_tag_t *name_tag =
                (mb2_string_tag_t *)tag;

            bootloader_name = name_tag->string;
        }

        /* EFI 32-bit system table */
        if (tag->type == 11) {
            boot_mode = BOOT_MODE_UEFI32;
        }

        /* EFI 64-bit system table */
        if (tag->type == 12) {
            boot_mode = BOOT_MODE_UEFI64;
        }

        /* Move to next 8-byte-aligned tag */
        ptr += (tag->size + 7u) & ~7u;
    }

    /*
     * With this GRUB Multiboot2 setup:
     * no EFI system-table tag means legacy BIOS boot.
     */
    if (boot_mode == BOOT_MODE_UNKNOWN) {
        boot_mode = BOOT_MODE_BIOS;
    }
}

const char *boot_get_bootloader_name(void)
{
    return bootloader_name;
}

boot_mode_t boot_get_mode(void)
{
    return boot_mode;
}