#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PAGE_SIZE 4096u
#define FRAME_COUNT 1048576u

int pmm_init(void *multiboot_info);

int32_t pmm_alloc_frame(void);
void pmm_free_frame(uint32_t frame);

void pmm_reserve_region(uint64_t addr, uint64_t len);

uint32_t pmm_get_free_frames(void);

#endif