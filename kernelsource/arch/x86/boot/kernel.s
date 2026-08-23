section .text
global _start
extern kernel_main

_start:
    push ebx          ; push multiboot info pointer
    mov esp, stack_top
    call kernel_main
    hlt

section .bss
stack_bottom:
    resb 16384
stack_top: