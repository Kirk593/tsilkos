BITS 32

section .text

global syscall_interrupt_stub
extern syscall_handler

syscall_interrupt_stub:
    pushad

    cld
    call syscall_handler

    popad
    iretd