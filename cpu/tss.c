#include <tss.h>
#include <gdt.h>
#include <string.h>


#define KERNEL_STACK_SIZE 8192
static uint8_t kernel_stack[KERNEL_STACK_SIZE] __attribute__((aligned(16)));

static tss_t tss_entry __attribute__((aligned(16)));

void tss_init(void)
{
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.rsp0 = (uint64_t)(kernel_stack + KERNEL_STACK_SIZE);

    extern void gdt_set_tss_entry(int idx, uint64_t base, uint32_t limit);
    gdt_set_tss_entry(5, (uint64_t)&tss_entry, sizeof(tss_entry) - 1);

    __asm__ __volatile__("ltr %%ax" :: "a"((uint16_t)(5 * 8)));
}