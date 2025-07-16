#include <gdt.h>
#include <vga.h>
#include <paging.h>
#include <tss.h>
#include <idt.h>
#include <entix.h>
#include <pic.h>
#include <string.h>
#include <cpu.h>
#include <debug.h>
#include <ps2.h>

void kernel_main(void)
{
    gdt_init();
    tss_init();
    kprintf("\n<(0F)>%s %s kernel\n\n", KERNEL_FNAME, KERNEL_VERSION);
    idt_init();
    kdbg(KINFO, "kernel_main: base success\n");
    kdbg(KINFO, "pic_remap: remapping 0x20, 0x28\n");
    pic_remap(0x20, 0x28);
    kdbg(KINFO, "pic_set_mask: masking all irq except keyboard (irq33)\n");
    for (uint8_t irq = 0; irq < 8; ++irq) {
        if (irq != 1) pic_set_mask(irq);
    }
    for (uint8_t irq = 8; irq < 16; ++irq) {
        pic_set_mask(irq);
    }
    __asm__("sti");
    paging_init();
    kprintf("%s %s shell\n\n", KERNEL_NAME, KERNEL_VERSION);
    while (1) {
        kprintf("entix> ");
        char *buf = kgets();
        if (strcmp(buf, "exit") == 0) {
            break;
        }
        else {
            kprintf("You typed: %s\n", buf);
        }
    }
    kdbg(KWARN, "kernel end. You may now shutdown");
    for (;;);
}