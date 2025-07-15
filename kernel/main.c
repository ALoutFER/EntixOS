#include <gdt.h>
#include <vga.h>
#include <paging.h>
#include <tss.h>
#include <idt.h>
#include <pic.h>

void kernel_main(void)
{
    gdt_init();
    tss_init();
    idt_init();
    kprintf("ENTiX 1.01\nkernel_main: base success\n");
    kprintf("pic_remap: remapping 0x20, 0x28\n");
    pic_remap(0x20, 0x28);
    kprintf("pic_set_mask: masking all irq except keyboard (irq1)\n");
    for (uint8_t irq = 0; irq < 8; ++irq) {
        if (irq != 1) pic_set_mask(irq);
    }
    for (uint8_t irq = 8; irq < 16; ++irq) {
        pic_set_mask(irq);
    }
    paging_init();
    __asm__("sti");
    kprintf("kernel end. You may now shutdown");
    for (;;);
} 