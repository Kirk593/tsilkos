static unsigned char heap[1024 * 1024];
static unsigned int heap_pos = 0;

void *kmalloc(unsigned int size) {
    if (heap_pos + size > sizeof(heap)) {
        return 0;
    }

    void *ptr = &heap[heap_pos];
    heap_pos += size;
    return ptr;
}