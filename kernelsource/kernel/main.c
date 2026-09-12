#include "bootstrap.h"
#include "video/fb.h"
#include <stdint.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36D76289u
#define MULTIBOOT2_TAG_END          0u
#define MULTIBOOT2_TAG_FRAMEBUFFER  8u
#define MULTIBOOT2_FB_TYPE_RGB      1u

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
    __asm__ volatile ("cli");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void kernel_main(uint32_t multiboot_magic, void *multiboot_info)
{
    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC || !multiboot_info)
        halt_forever();

    Multiboot2Info *info = (Multiboot2Info *)multiboot_info;
    if (info->total_size < 16u)
        halt_forever();

    kernel_bootstrap();

    uint8_t *ptr = (uint8_t *)multiboot_info + 8u;
    uint8_t *end = (uint8_t *)multiboot_info + info->total_size;

    while (ptr + sizeof(Multiboot2Tag) <= end) {
        Multiboot2Tag *tag = (Multiboot2Tag *)ptr;

        if (tag->type == MULTIBOOT2_TAG_END)
            break;

        if (tag->size < sizeof(Multiboot2Tag) || ptr + tag->size > end)
            break;

        if (tag->type == MULTIBOOT2_TAG_FRAMEBUFFER &&
            tag->size >= sizeof(Multiboot2FramebufferTag)) {
            Multiboot2FramebufferTag *framebuffer =
                (Multiboot2FramebufferTag *)tag;

            if (framebuffer->framebuffer_bpp == 32u &&
                framebuffer->framebuffer_type == MULTIBOOT2_FB_TYPE_RGB &&
                (uint32_t)(framebuffer->framebuffer_addr >> 32) == 0u) {
                fb_init(
                    (void *)(uintptr_t)(uint32_t)framebuffer->framebuffer_addr,
                    framebuffer->framebuffer_width,
                    framebuffer->framebuffer_height,
                    framebuffer->framebuffer_pitch
                );

                draw_string("kernel booting!", 100, 100, 0xFFFFFFu);
                draw_string("Multiboot2 framebuffer: OK", 100, 116, 0x00FF00u);
            }

            break;
        }

        ptr += (tag->size + 7u) & ~7u;
    }

    halt_forever();
}
