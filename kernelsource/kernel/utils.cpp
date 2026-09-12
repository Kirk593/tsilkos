#include "utils.h"

extern "C" uintptr_t align_up(uintptr_t value, uintptr_t alignment)
{
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        return value;
    }

    return (value + (alignment - 1)) & ~(alignment - 1);
}
