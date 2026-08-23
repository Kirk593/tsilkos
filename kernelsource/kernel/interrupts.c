#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} IDTEntry;

IDTEntry idt[256];

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDTPointer;

void idt_set_entry(int num, uint32_t handler) {
    idt[num].offset_low  = handler & 0xFFFF;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
    idt[num].selector    = 0x08;
    idt[num].zero        = 0;
    idt[num].type_attr   = 0x8E;
}

void idt_load() {
    IDTPointer ptr;
    ptr.limit = sizeof(idt) - 1;
    ptr.base  = (uint32_t)idt;
    __asm__("lidt %0" : : "m"(ptr));
}