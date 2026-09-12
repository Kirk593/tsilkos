#ifndef KERNEL_UTILS_H
#define KERNEL_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* alignment must be a non-zero power of two. */
uintptr_t align_up(uintptr_t value, uintptr_t alignment);

#ifdef __cplusplus
}
#endif

#endif
