#include "interrupts.h"
#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) IDTEntry;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDTPointer;

static IDTEntry idt[256];

static uint16_t current_code_selector(void)
{
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    return cs;
}

void idt_set_entry(uint8_t vector, uint32_t handler, uint8_t type_attr)
{
    idt[vector].offset_low  = (uint16_t)(handler & 0xFFFFu);
    idt[vector].selector    = current_code_selector();
    idt[vector].zero        = 0;
    idt[vector].type_attr   = type_attr;
    idt[vector].offset_high = (uint16_t)((handler >> 16) & 0xFFFFu);
}

void idt_init(void)
{
    for (unsigned int i = 0; i < 256; ++i) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }
}

void idt_load(void)
{
    IDTPointer ptr;
    ptr.limit = (uint16_t)(sizeof(idt) - 1u);
    ptr.base  = (uint32_t)(uintptr_t)idt;
    __asm__ volatile ("lidt %0" : : "m"(ptr) : "memory");
}
