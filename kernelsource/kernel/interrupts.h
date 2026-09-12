#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

void idt_init(void);
void idt_load(void);

#ifdef __cplusplus
}
#endif

#endif