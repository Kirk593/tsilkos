BITS 32

section .multiboot
align 8

mb2_header:
    dd 0xE85250D6
    dd 0
    dd mb2_header_end - mb2_header
    dd -(0xE85250D6 + 0 + (mb2_header_end - mb2_header))

    ; Ask GRUB for any framebuffer it can provide
    align 8
    dw 5
    dw 1
    dd 20
    dd 0
    dd 0
    dd 0

    ; End tag
    align 8
    dw 0
    dw 0
    dd 8

mb2_header_end:


section .text
global _start
extern kernel_main

_start:
    cli

    mov esp, stack_top

    ; Multiboot2:
    ; EAX = 0x36D76289
    ; EBX = Multiboot2 info pointer
    push ebx
    push eax

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang


section .bss
align 16

stack_bottom:
    resb 16384

stack_top: