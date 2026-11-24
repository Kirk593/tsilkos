
#include <stdint.h>

#define PAGE_SIZE   4096u
#define FRAME_COUNT 32u

static uint8_t bitmap[FRAME_COUNT / 8];

static void pmm_set(uint32_t frame)
{
    bitmap[frame / 8] |= (1u << (frame % 8));
}

static void pmm_clear(uint32_t frame)
{
    bitmap[frame / 8] &= ~(1u << (frame % 8));
}

static int pmm_test(uint32_t frame)
{
    return (bitmap[frame / 8] &
           (1u << (frame % 8))) != 0;
}

int pmm_alloc_frame(void)
{
    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        if (pmm_test(i) == 0) {
            pmm_set(i);
            return i;
        }
    }

    return -1;
}

void pmm_free_frame(uint32_t frame)
{
    if (frame < FRAME_COUNT) {
        pmm_clear(frame);
    }
}
