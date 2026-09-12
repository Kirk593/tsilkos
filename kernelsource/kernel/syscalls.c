#include "syscalls.h"
#include "interrupts.h"
#include "video/fb.h"
#include <stdint.h>

typedef void (*syscall_fn)(void);

static void sys_exit(void)
{
    __asm__ volatile ("cli");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

static void sys_write(void)
{
    draw_string("syscall: output", 100, 132, 0xFFFFFFu);
}

static syscall_fn syscall_table[] = {
    sys_exit,
    sys_write,
};

void syscall_dispatch(uint32_t syscall_num)
{
    const uint32_t count = (uint32_t)(sizeof(syscall_table) / sizeof(syscall_table[0]));
    if (syscall_num < count)
        syscall_table[syscall_num]();
}

void syscalls_init(void)
{
    /* Present, 32-bit interrupt gate. DPL 0 for now (kernel callers only). */
    idt_set_entry(0x80u, (uint32_t)(uintptr_t)syscall_interrupt_stub, 0x8Eu);
}
