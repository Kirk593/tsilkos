extern "C" {
#include "interrupts.h"
#include "syscalls.h"
}

#include "bootstrap.h"

/* Keep bootstrap implementation private to this translation unit, similar
 * to the pattern used by larger kernels such as XNU. */
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
    /* kmalloc currently uses a statically reserved heap, so it requires
     * no runtime initialization yet. */
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
    /* Filesystems should be mounted only after a block device is found. */
}
