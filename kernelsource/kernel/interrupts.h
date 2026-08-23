#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void idt_set_entry(int num, uint32_t handler);
void idt_load();

#endif