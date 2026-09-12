CC  = gcc
CXX = g++
AS  = nasm
LD  = ld

CPPFLAGS = -Ikernel -Ikernel/video -Ikernel/fs

COMMON = -m32 \
         -ffreestanding \
         -fno-stack-protector \
         -fno-pie \
         -fno-pic \
         -fno-unwind-tables \
         -fno-asynchronous-unwind-tables \
         -Wall \
         -Wextra

CFLAGS   = $(COMMON) -std=gnu11
CXXFLAGS = $(COMMON) -std=gnu++17 \
           -fno-exceptions \
           -fno-rtti \
           -fno-use-cxa-atexit

NASMFLAGS = -f elf32
GASFLAGS  = -m32 -ffreestanding -fno-pie -fno-pic

LDFLAGS = -m elf_i386 -T linker.ld

OBJS = \
	build/kernel_entry.o \
	build/syscall_stub.o \
	build/main.o \
	build/bootstrap.o \
	build/utils.o \
	build/fb.o \
	build/interrupts.o \
	build/syscalls.o \
	build/kmalloc.o \
	build/ata.o \
	build/ext2.o

all: kernel.bin

kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

build:
	mkdir -p build

# Your kernel.s is NASM syntax
build/kernel_entry.o: arch/x86/boot/kernel.s | build
	$(AS) $(NASMFLAGS) $< -o $@

# Keep this with GCC if syscall.s is still GNU/GAS syntax
build/syscall_stub.o: arch/x86/boot/syscall.s | build
	$(CC) $(GASFLAGS) -c $< -o $@

build/main.o: kernel/main.c kernel/bootstrap.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/bootstrap.o: kernel/bootstrap.cpp kernel/bootstrap.h | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/utils.o: kernel/utils.cpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

build/fb.o: kernel/video/fb.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/interrupts.o: kernel/interrupts.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/syscalls.o: kernel/syscalls.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/kmalloc.o: kernel/kmalloc.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/ata.o: kernel/fs/ata.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/ext2.o: kernel/fs/ext2.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build kernel.bin

.PHONY: all clean