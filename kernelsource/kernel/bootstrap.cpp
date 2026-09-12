extern "C" {
#include "bootstrap.h"
#include "syscalls.h"
#include "interrupts.h"
}

static void bootstrapInterrupts(void);
static void bootstrapSyscalls(void);
static void bootstrapMemory(void);
static void bootstrapDevices(void);
static void bootstrapFilesystem(void);

extern "C" void kernel_bootstrap(void)
{
    bootstrapMemory();
    bootstrapInterrupts();
    bootstrapSyscalls();
    bootstrapDevices();
    bootstrapFilesystem();

    idt_load();
}

static void bootstrapMemory(void)
{
    /* kmalloc currently uses a statically reserved heap. */
}

static void bootstrapInterrupts(void)
{
    idt_init();
}

static void bootstrapSyscalls(void)
{
    syscalls_init();
}

static void bootstrapDevices(void)
{
    /* PCI/AHCI/NVMe discovery can be wired in here later. */
}

static void bootstrapFilesystem(void)
{
    /* Mount filesystems only after a block device is discovered. */
}
