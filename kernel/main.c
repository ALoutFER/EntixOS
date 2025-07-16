#include <gdt.h>
#include <vga.h>
#include <paging.h>
#include <tss.h>
#include <idt.h>
#include <entix.h>
#include <pic.h>
#include <string.h>
#include <cpu.h>
#include <heap.h>
#include <debug.h>
#include <ps2.h>

void kernel_main(uint32_t magic, uint32_t addr)
{
    gdt_init();
    tss_init();
    kprintf("\n<(0F)>%s %s Operating System\n\n", KERNEL_FNAME, KERNEL_VERSION);
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
    heap_init(HEAP_START, HEAP_SIZE);
    kdbg(KDBG, "heap_init: HEAP_START=%x, HEAP_SIZE=%x\n", HEAP_START, HEAP_SIZE);

    void *ptr = kmalloc(1024);
    kdbg(KDBG, "kmalloc: ptr=%x\n", ptr);

    void *ptr2 = kmalloc(1024);
    kdbg(KDBG, "kmalloc: ptr2=%x\n", ptr2);
    kfree(ptr);
    kfree(ptr2);
    kdbg(KDBG, "kfree: ptr=%x\n", ptr);
    kdbg(KDBG, "kfree: ptr2=%x\n", ptr2);

    kprintf("%s %s shell\n\n", KERNEL_NAME, KERNEL_VERSION);
    while (1) {
        kprintf("entix> ");
        char *buf = kgets();
        if (strcmp(buf, "exit") == 0) {
            break;
        }
        else if (strcmp(buf, "help") == 0) {
            kprint("help command\n");
        }
        else {
            kprintf("You typed: %s\n", buf);
        }
    }
    kdbg(KWARN, "kernel end. You may now shutdown");
    for (;;);
}