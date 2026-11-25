
#include "bootstrap.h"
#include "boot.h"
#include "video/fb.h"
#include "pmm.h"
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
    // Validate Multiboot2 boot information.
    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC ||
        multiboot_info == 0) {
        halt_forever();
    }

    Multiboot2Info *info =
        (Multiboot2Info *)multiboot_info;

    if (info->total_size < 16u) {
        halt_forever();
    }

    // Prevent address overflow.
    if (info->total_size >
        UINTPTR_MAX - (uintptr_t)multiboot_info) {
        halt_forever();
    }

    // Detect bootloader and boot mode.
    boot_detect(multiboot_info);

    // Initialize physical memory manager.
    int pmm_ready =
        (pmm_init(multiboot_info) == 0);

    // Only bootstrap if PMM initialization succeeded.
    if (pmm_ready) {
        kernel_bootstrap();
    }

    // Parse Multiboot2 tags.
    uint8_t *base = (uint8_t *)multiboot_info;
    uint32_t offset = 8u;
    uint32_t total_size = info->total_size;

    int framebuffer_ready = 0;

    while (total_size - offset >=
           sizeof(Multiboot2Tag)) {

        Multiboot2Tag *tag =
            (Multiboot2Tag *)(base + offset);

        // Validate tag size.
        if (tag->size < sizeof(Multiboot2Tag) ||
            tag->size > total_size - offset) {
            break;
        }

        // End of Multiboot2 tags.
        if (tag->type == MB2_TAG_END) {
            break;
        }

        // Locate and validate framebuffer tag.
        if (tag->type == MB2_TAG_FRAMEBUFFER &&
            tag->size >= sizeof(Multiboot2FramebufferTag)) {

            Multiboot2FramebufferTag *framebuffer =
                (Multiboot2FramebufferTag *)tag;

            // Support 32-bit RGB framebuffers.
            if (framebuffer->framebuffer_type ==
                    MB2_FB_TYPE_RGB &&
                framebuffer->framebuffer_bpp == 32u) {

                uint64_t fb_addr =
                    framebuffer->framebuffer_addr;

                uint64_t fb_size =
                    (uint64_t)framebuffer->framebuffer_pitch *
                    framebuffer->framebuffer_height;

                // Ensure framebuffer fits below 4 GiB.
                if (fb_addr != 0 &&
                    fb_addr < 0x100000000ULL &&
                    fb_size != 0 &&
                    fb_size <= 0x100000000ULL - fb_addr &&
                    framebuffer->framebuffer_width != 0 &&
                    (uint64_t)framebuffer->framebuffer_width * 4u
                        <= framebuffer->framebuffer_pitch) {

                    fb_init(
                        (void *)(uintptr_t)fb_addr,
                        framebuffer->framebuffer_width,
                        framebuffer->framebuffer_height,
                        framebuffer->framebuffer_pitch
                    );

                    framebuffer_ready = 1;
                }
            }
        }

        // Advance to the next 8-byte aligned tag.
        uint64_t step =
            ((uint64_t)tag->size + 7u) & ~7ULL;

        if (step > total_size - offset) {
            break;
        }

        offset += (uint32_t)step;
    }

    // Display kernel boot information.
    if (framebuffer_ready) {

        draw_string(
            "TsilkOS booting!",
            100, 100, 0xFFFFFFu
        );

        draw_string(
            "Multiboot2 framebuffer: OK",
            100, 116, 0x00FF00u
        );

        draw_string(
            "Bootloader:",
            100, 140, 0xFFFFFFu
        );

        draw_string(
            boot_get_bootloader_name(),
            196, 140, 0xFFFFFFu
        );

        boot_mode_t mode = boot_get_mode();

        if (mode == BOOT_MODE_UEFI64) {

            draw_string(
                "Boot mode: UEFI 64-bit",
                100, 156, 0x00FF00u
            );

        } else if (mode == BOOT_MODE_UEFI32) {

            draw_string(
                "Boot mode: UEFI 32-bit",
                100, 156, 0x00FF00u
            );

        } else if (mode == BOOT_MODE_BIOS) {

            draw_string(
                "Boot mode: Legacy BIOS",
                100, 156, 0xFFFF00u
            );

        } else {

            draw_string(
                "Boot mode: Unknown",
                100, 156, 0xFF0000u
            );
        }

        // Physical memory manager status.
        if (pmm_ready) {

            draw_string(
                "PMM: ONLINE",
                100, 180, 0x00FF00u
            );

            // Test physical frame allocation.
            uint32_t before = pmm_get_free_frames();

            int32_t frame = pmm_alloc_frame();

            if (frame >= 0) {

                uint32_t after = pmm_get_free_frames();

                pmm_free_frame((uint32_t)frame);

                uint32_t restored = pmm_get_free_frames();

                if (before > 0 &&
                    after == before - 1 &&
                    restored == before) {

                    draw_string(
                        "Memory allocation: OK",
                        100, 196, 0x00FF00u
                    );

                } else {

                    draw_string(
                        "Memory accounting: FAILED",
                        100, 196, 0xFF0000u
                    );
                }

            } else {

                draw_string(
                    "Memory allocation: FAILED",
                    100, 196, 0xFF0000u
                );
            }

        } else {

            draw_string(
                "PMM: INITIALIZATION FAILED",
                100, 180, 0xFF0000u
            );
        }
    }

    halt_forever();
}
