#include <gdt.h>
#include <vga.h>
#include <paging.h>
#include <tss.h>
#include <idt.h>
#include <pic.h>
#include <cpu.h>
#include <ps2.h>

void kernel_main(void)
{
    gdt_init();
    tss_init();
    kprintf("\nENT!X 1.01 kernel\n\n");
    idt_init();
    kprintf("kernel_main: base success\n");
    kprintf("pic_remap: remapping 0x20, 0x28\n");
    pic_remap(0x20, 0x28);
    kprintf("pic_set_mask: masking all irq except keyboard (irq33)\n");
    for (uint8_t irq = 0; irq < 8; ++irq) {
        if (irq != 1) pic_set_mask(irq);
    }
    for (uint8_t irq = 8; irq < 16; ++irq) {
        pic_set_mask(irq);
    }
    __asm__("sti");
    paging_init();
    while (1) {
        kprintf("> ");
        char *buf = kgets();
        kprintf("you typed: %s\n", buf);
    }
    kprintf("kernel end. You may now shutdown");
    for (;;);
}