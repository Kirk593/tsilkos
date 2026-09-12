CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib
ASFLAGS = -f elf32

all: kernel.bin

kernel.bin: kernel.o main.o fb.o interrupts.o
	$(LD) -m elf_i386 -T linker.ld -o kernel.bin kernel.o main.o fb.o interrupts.o

kernel.o: arch/x86/kernel.s
	$(AS) $(ASFLAGS) arch/x86/boot/kernel.s -o kernel.o

main.o: kernel/main.c
	$(CC) $(CFLAGS) -c kernel/main.c -o main.o

fb.o: kernel/video/fb.c
	$(CC) $(CFLAGS) -I kernel/video -I kernel/video/font_framebuffer -c kernel/video/fb.c -o fb.o

interrupts.o: kernel/interrupts.c
	$(CC) $(CFLAGS) -c kernel/interrupts.c -o interrupts.o

clean:
	rm -f *.o kernel.bin