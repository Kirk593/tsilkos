
#include "pmm.h"
#include <stddef.h>

#define BITMAP_SIZE (FRAME_COUNT / 8u)
#define MEMORY_LIMIT 0x100000000ULL
#define MB2_TAG_MMAP 6u

extern char kernel_start[];
extern char kernel_end[];

static uint8_t bitmap[BITMAP_SIZE];
static uint8_t reserved[BITMAP_SIZE];
static uint32_t free_frames;
static int initialized;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} __attribute__((packed)) MmapTag;

typedef struct {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) MmapEntry;

static int pmm_test(uint32_t frame)
{
    return (bitmap[frame / 8] &
           (1u << (frame % 8))) != 0;
}

static void pmm_set(uint32_t frame)
{
    if (!pmm_test(frame)) {
        bitmap[frame / 8] |=
            (1u << (frame % 8));

        free_frames--;
    }
}

static void pmm_clear(uint32_t frame)
{
    if (pmm_test(frame)) {
        bitmap[frame / 8] &=
            ~(1u << (frame % 8));

        free_frames++;
    }
}

static uint64_t region_end(uint64_t addr,
                           uint64_t len)
{
    if (addr >= MEMORY_LIMIT)
        return MEMORY_LIMIT;

    if (len > MEMORY_LIMIT - addr)
        return MEMORY_LIMIT;

    return addr + len;
}

void pmm_reserve_region(uint64_t addr,
                        uint64_t len)
{
    if (!len || addr >= MEMORY_LIMIT)
        return;

    uint64_t end = region_end(addr, len);

    uint32_t first = addr / PAGE_SIZE;
    uint32_t last =
        (end + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = first; i < last; i++) {
        reserved[i / 8] |=
            (1u << (i % 8));

        pmm_set(i);
    }
}

static void pmm_release_region(uint64_t addr,
                               uint64_t len)
{
    if (!len || addr >= MEMORY_LIMIT)
        return;

    uint64_t end = region_end(addr, len);

    uint32_t first =
        (addr + PAGE_SIZE - 1) / PAGE_SIZE;

    uint32_t last = end / PAGE_SIZE;

    for (uint32_t i = first; i < last; i++) {
        reserved[i / 8] &=
            ~(1u << (i % 8));

        pmm_clear(i);
    }
}

int pmm_init(void *multiboot_info)
{
    initialized = 0;
    free_frames = 0;

    // Initially reserve all physical memory.
    for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0xFF;
        reserved[i] = 0xFF;
    }

    if (!multiboot_info)
        return -1;

    uint8_t *base = multiboot_info;
    uint32_t total_size = *(uint32_t *)base;

    if (total_size < 16)
        return -1;

    int found_mmap = 0;

    // Pass 0: release available RAM.
    // Pass 1: reserve unavailable regions.
    for (int pass = 0; pass < 2; pass++) {
        uint32_t offset = 8;

        while (total_size - offset >= 8) {
            uint32_t *header =
                (uint32_t *)(base + offset);

            uint32_t type = header[0];
            uint32_t size = header[1];

            if (size < 8 ||
                size > total_size - offset)
                return -1;

            if (type == 0)
                break;

            if (type == MB2_TAG_MMAP) {
                MmapTag *tag =
                    (MmapTag *)(base + offset);

                if (size < sizeof(MmapTag) ||
                    tag->entry_size < sizeof(MmapEntry))
                    return -1;

                found_mmap = 1;

                uint32_t pos = sizeof(MmapTag);

                while (size - pos >= tag->entry_size) {
                    MmapEntry *entry =
                        (MmapEntry *)(base + offset + pos);

                    if (pass == 0 && entry->type == 1) {
                        pmm_release_region(
                            entry->addr,
                            entry->len
                        );
                    }

                    if (pass == 1 && entry->type != 1) {
                        pmm_reserve_region(
                            entry->addr,
                            entry->len
                        );
                    }

                    pos += tag->entry_size;
                }
            }

            // Reserve boot modules.
            if (pass == 1 && type == 3) {
                if (size < 16)
                    return -1;

                uint32_t start = header[2];
                uint32_t end = header[3];

                if (end > start)
                    pmm_reserve_region(start, end - start);
            }

            // Reserve framebuffer memory.
            if (pass == 1 && type == 8) {
                if (size < 32)
                    return -1;

                uint64_t addr =
                    *(uint64_t *)(base + offset + 8);

                uint32_t pitch = header[4];
                uint32_t height = header[6];

                pmm_reserve_region(
                    addr,
                    (uint64_t)pitch * height
                );
            }

            uint64_t step =
                ((uint64_t)size + 7) & ~7ULL;

            if (step > total_size - offset)
                return -1;

            offset += (uint32_t)step;
        }
    }

    if (!found_mmap)
        return -1;

    // Reserve the first MiB.
    pmm_reserve_region(0, 0x100000);

    // Protect the kernel image.
    pmm_reserve_region(
        (uintptr_t)kernel_start,
        (uintptr_t)kernel_end -
        (uintptr_t)kernel_start
    );

    // Protect Multiboot2 information.
    pmm_reserve_region(
        (uintptr_t)multiboot_info,
        total_size
    );

    initialized = 1;
    return 0;
}

int32_t pmm_alloc_frame(void)
{
    if (!initialized)
        return -1;

    for (uint32_t i = 0; i < FRAME_COUNT; i++) {
        if (!pmm_test(i)) {
            pmm_set(i);
            return (int32_t)i;
        }
    }

    return -1;
}

void pmm_free_frame(uint32_t frame)
{
    if (!initialized || frame >= FRAME_COUNT)
        return;

    if (reserved[frame / 8] &
        (1u << (frame % 8)))
        return;

    pmm_clear(frame);
}

uint32_t pmm_get_free_frames(void)
{
    return initialized ? free_frames : 0;
}
