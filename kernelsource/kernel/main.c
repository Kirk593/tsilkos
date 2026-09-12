#include "bootstrap.h"
#include "video/fb.h"
#include <stdint.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36D76289u
#define MULTIBOOT2_TAG_END          0
#define MULTIBOOT2_TAG_FRAMEBUFFER  8

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
} __attribute__((packed)) Multiboot2FramebufferTag;


static void halt_forever(void)
{
    __asm__ volatile("cli");

    for (;;) {
        __asm__ volatile("hlt");
    }
}


void kernel_main(uint32_t multiboot_magic, void *multiboot_info)
{
    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC ||
        multiboot_info == 0) {
        halt_forever();
    }

    kernel_bootstrap();

    uint8_t *ptr = (uint8_t *)multiboot_info + 8;

    for (;;) {
        Multiboot2Tag *tag = (Multiboot2Tag *)ptr;

        if (tag->type == MULTIBOOT2_TAG_END)
            break;

        if (tag->type == MULTIBOOT2_TAG_FRAMEBUFFER) {

            Multiboot2FramebufferTag *fb =
                (Multiboot2FramebufferTag *)tag;

            if (fb->framebuffer_bpp == 32 &&
                (uint32_t)(fb->framebuffer_addr >> 32) == 0) {

                fb_init(
                    (void *)(uintptr_t)(uint32_t)fb->framebuffer_addr,
                    fb->framebuffer_width,
                    fb->framebuffer_height,
                    fb->framebuffer_pitch
                );

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
            }

            break;
        }

        /*
         * Every Multiboot2 tag is aligned to 8 bytes.
         */
        ptr += (tag->size + 7u) & ~7u;
    }

    halt_forever();
}