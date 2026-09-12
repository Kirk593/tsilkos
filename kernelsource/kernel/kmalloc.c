#include "kmalloc.h"
#include "utils.h"
#include <stdint.h>

static unsigned char heap[1024 * 1024] __attribute__((aligned(16)));
static uintptr_t heap_pos;

void *kmalloc(unsigned int size)
{
    if (size == 0)
        return 0;

    uintptr_t start = align_up(heap_pos, 16u);
    if (start > sizeof(heap) || size > sizeof(heap) - start)
        return 0;

    void *ptr = &heap[start];
    heap_pos = start + size;
    return ptr;
}
