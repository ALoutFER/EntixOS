#ifndef PS2_H
#define PS2_H

#include <stdint.h>
#include <cpu.h>

//push a received character into keyboard buff
void keyboard_buffer_push(char c);

//pop a char from keyboard buffer; returns 1 if char available, 0 otherw
int keyboard_buffer_pop(char *c);

static void keyboard_handler(cpu_registers_t* regs);

void ps2_init(void);
char kgetch(void);
char *kgets();

#endif // PS2_H 