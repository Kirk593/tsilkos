#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void idt_init(void);
void idt_load(void);
void idt_set_entry(uint8_t vector, uint32_t handler, uint8_t type_attr);

#ifdef __cplusplus
}
#endif

#endif