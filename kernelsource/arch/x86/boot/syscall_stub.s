BITS 32

section .text

global syscall_interrupt_stub
extern syscall_dispatch

syscall_interrupt_stub:
    pushad

    push eax
    call syscall_dispatch
    add esp, 4

    popad
    iretd