typedef void (*syscall_fn)();

void sys_write() {
    draw_string("output", 100, 100, 0xFFFFFF);
}

void sys_exit() {
    __asm__("hlt");
}

syscall_fn syscall_table[] = {
    sys_exit,
    sys_write,
};

void syscall_handler() {
    int syscall_num;
    __asm__("mov %%eax, %0" : "=r"(syscall_num));

    if (syscall_num < 8) {
        syscall_table[syscall_num]();
    }
}

void idt_init() {
    idt_set_entry(0x80, (uint32_t)syscall_handler);
    idt_load();
}