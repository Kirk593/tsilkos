#include "bootstrap.h"
#include "boot.h"
#include "video/fb.h"
#include <stdint.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36D76289u

#define MB2_TAG_END          0u
#define MB2_TAG_FRAMEBUFFER  8u

#define MB2_FB_TYPE_RGB      1u


typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed)) Multiboot2Info;


typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) Multiboot2Tag;


typedef struct {
    uint32_t type;
    uint32_t size;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;

    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t reserved;

    /* RGB framebuffer information */
    uint8_t red_position;
    uint8_t red_mask_size;

    uint8_t green_position;
    uint8_t green_mask_size;

    uint8_t blue_position;
    uint8_t blue_mask_size;

} __attribute__((packed)) Multiboot2FramebufferTag;


static void halt_forever(void)
{
    __asm__ volatile ("cli");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}


void kernel_main(
    uint32_t multiboot_magic,
    void *multiboot_info
)
{
    /*
     * Make sure GRUB actually booted us using Multiboot2.
     */
    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC ||
        multiboot_info == 0) {

        halt_forever();
    }


    Multiboot2Info *info =
        (Multiboot2Info *)multiboot_info;


    if (info->total_size < 16u) {
        halt_forever();
    }


    /*
     * Detect GRUB / BIOS / UEFI.
     */
    boot_detect(multiboot_info);


    /*
     * Initialize the rest of the kernel.
     */
    kernel_bootstrap();


    /*
     * Multiboot2 tags begin after:
     *
     * uint32_t total_size
     * uint32_t reserved
     *
     * = 8 bytes
     */
    uint8_t *ptr =
        (uint8_t *)multiboot_info + 8u;

    uint8_t *end =
        (uint8_t *)multiboot_info +
        info->total_size;


    int framebuffer_ready = 0;


    while (ptr + sizeof(Multiboot2Tag) <= end) {

        Multiboot2Tag *tag =
            (Multiboot2Tag *)ptr;


        /*
         * End of Multiboot2 tags.
         */
        if (tag->type == MB2_TAG_END) {
            break;
        }


        /*
         * Protect against a broken tag.
         */
        if (tag->size < sizeof(Multiboot2Tag)) {
            break;
        }


        if (ptr + tag->size > end) {
            break;
        }


        /*
         * Framebuffer information.
         */
        if (tag->type == MB2_TAG_FRAMEBUFFER) {

            Multiboot2FramebufferTag *framebuffer =
                (Multiboot2FramebufferTag *)tag;


            /*
             * Our framebuffer driver currently understands
             * 32-bit RGB only.
             */
            if (framebuffer->framebuffer_type == MB2_FB_TYPE_RGB &&
                framebuffer->framebuffer_bpp == 32u) {

                /*
                 * We're still running a 32-bit kernel.
                 *
                 * Therefore we can only directly use framebuffer
                 * addresses below 4 GiB.
                 */
                if ((framebuffer->framebuffer_addr >> 32) == 0u) {

                    fb_init(
                        (void *)(uintptr_t)
                            framebuffer->framebuffer_addr,

                        framebuffer->framebuffer_width,
                        framebuffer->framebuffer_height,
                        framebuffer->framebuffer_pitch
                    );

                    framebuffer_ready = 1;
                }
            }
        }


        /*
         * Multiboot2 tags are aligned to 8-byte boundaries.
         */
        ptr += (tag->size + 7u) & ~7u;
    }


    /*
     * Only draw after framebuffer setup succeeded.
     */
    if (framebuffer_ready) {

        draw_string(
            "kernel booting!",
            100,
            100,
            0xFFFFFFu
        );


        draw_string(
            "Multiboot2 framebuffer: OK",
            100,
            116,
            0x00FF00u
        );


        draw_string(
            "Bootloader:",
            100,
            140,
            0xFFFFFFu
        );


        draw_string(
            boot_get_bootloader_name(),
            196,
            140,
            0xFFFFFFu
        );


        boot_mode_t mode =
            boot_get_mode();


        if (mode == BOOT_MODE_UEFI64) {

            draw_string(
                "Boot mode: UEFI 64-bit",
                100,
                156,
                0x00FF00u
            );

        } else if (mode == BOOT_MODE_UEFI32) {

            draw_string(
                "Boot mode: UEFI 32-bit",
                100,
                156,
                0x00FF00u
            );

        } else if (mode == BOOT_MODE_BIOS) {

            draw_string(
                "Boot mode: Legacy BIOS",
                100,
                156,
                0xFFFF00u
            );

        } else {

            draw_string(
                "Boot mode: Unknown",
                100,
                156,
                0xFF0000u
            );
        }
    }


    halt_forever();
}