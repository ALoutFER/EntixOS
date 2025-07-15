#include <idt.h>
#include <gdt.h>
#include <pic.h>
#include <port_based.h>
#include <stddef.h>
#include <cpu.h>
#include <vga.h>
#include <string.h>

#define IDT_SIZE 256

idt_entry_t idt[IDT_SIZE] __attribute__((aligned(16)));
idt_descriptor_t idtr;

void (*interrupt_handlers[IDT_SIZE])(cpu_registers_t*) = {0};

extern void *isr_stub_table[];

static void idt_set_gate(uint8_t vector, uint64_t handler, uint8_t flags)
{
    idt[vector].offset_low  = (uint16_t)(handler & 0xFFFF);
    idt[vector].selector    = 0x08;
    idt[vector].ist         = 0;
    idt[vector].type_attr   = flags;
    idt[vector].offset_mid  = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[vector].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    idt[vector].zero        = 0;
}

void isr_dispatch(cpu_registers_t* regs)
{
    uint8_t vec = (uint8_t)regs->int_no;
    switch (vec) {
        case 1:
            kprintf("Timer interrupt\n");
            break;
        case 46:
            kprintf("Page fault!\nEIP: %08X\n", regs->rip);
            break;
        case 33:
            kprintf("Keyboard interrupt\n");
            break;
        default:
            kprintf("\nkernel panic: %s\n", exception_messages[vec]);
            kprintf("RIP: 0x%016X\n", regs->rip);
            kprintf("kernel halted");
            for (;;);
    }
    if (interrupt_handlers[vec])
        interrupt_handlers[vec](regs);

    if (vec >= 32 && vec <= 47)
        pic_send_eoi(vec - 32);
    kprintf("\ninterrupt: %d\n", vec);
}

void idt_init(void)
{
    memset(idt, 0, sizeof(idt));
    for (int i = 0; i < IDT_SIZE; ++i) {
        uint8_t flags = 0x8E;
        if (i == 0x80) {
            flags = 0xEE;
        }
        idt_set_gate(i, (uint64_t)isr_stub_table[i], flags);
    }

    idtr.size = sizeof(idt) - 1;
    idtr.offset = (uint64_t)&idt;
    __asm__ __volatile__("lidt %0" : : "m"(idtr));
    kprintf("idt initialized\n");
}

void idt_register_handler(uint8_t vector, void (*handler)(cpu_registers_t*))
{
    interrupt_handlers[vector] = handler;
} 

void idt_uninstall_handler(uint8_t vector)
{
    interrupt_handlers[vector] = NULL;
}