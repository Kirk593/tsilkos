BITS 32

section .multiboot
align 8

mb2_header:
    ; Fixed Multiboot2 header
    dd 0xE85250D6                         ; magic
    dd 0                                  ; architecture = i386
    dd mb2_header_end - mb2_header        ; header length
    dd -(0xE85250D6 + 0 + (mb2_header_end - mb2_header))

    ; Request a framebuffer
    align 8
    dw 5              ; framebuffer header tag
    dw 0              ; flags
    dd 20             ; tag size
    dd 1024           ; width
    dd 768            ; height
    dd 32             ; bits per pixel

    ; Required end tag
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

    ; Multiboot2 gives:
    ; EAX = 0x36D76289
    ; EBX = pointer to Multiboot2 information

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