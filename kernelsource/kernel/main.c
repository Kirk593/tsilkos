#include "interrupts.h"
#include "video/fb.h"
#include <stdint.h>

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint64_t framebuffer_addr;   // ← this is what you need!
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
} MultibootInfo;

void kernel_main(MultibootInfo *mb) {
    fb_init((unsigned int *)mb->framebuffer_addr, 
             mb->framebuffer_width, 
             mb->framebuffer_height);
    idt_load();
    draw_string("kernel booting!", 100, 100, 0xFFFFFF);
}