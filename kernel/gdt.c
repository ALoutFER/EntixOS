#include <gdt.h>
#include <cpu.h>

#define GDT_ENTRIES 7

#define GDT_FLAG_GRANULARITY  0x8
#define GDT_FLAG_32BIT        0x4
#define GDT_FLAG_LONGMODE     0x2

#define GDT_ACCESS_RING0      0x00 /* kernel ring 0 (example: TempleOS by Terry Davis running in ring 0
                                    * that isn't safe) */

#define GDT_ACCESS_RING3      0x60 // kernel ring 3 (user mode), example: Windows from 9x .. now (11)
                                   // - allows safe running applications in user space

#define GDT_ACCESS_PRESENT    0x80
#define GDT_ACCESS_CODE       0x18
#define GDT_ACCESS_DATA       0x10
#define GDT_ACCESS_RW         0x02

static gdt_entry_t gdt[GDT_ENTRIES] __attribute__((aligned(8)));
static gdt_descriptor_t gdtr;

static void gdt_set_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[idx].limit_low = (uint16_t)(limit & 0xFFFF);
    gdt[idx].base_low  = (uint16_t)(base & 0xFFFF);
    gdt[idx].base_mid  = (uint8_t)((base >> 16) & 0xFF);
    gdt[idx].access    = access;
    gdt[idx].flags_limit = (uint8_t)(((limit >> 16) & 0x0F) | (flags << 4));
    gdt[idx].base_high = (uint8_t)((base >> 24) & 0xFF);
}

void gdt_init(void)
{
    gdtr.size = sizeof(gdt) - 1;
    gdtr.offset = (uint64_t)&gdt;

    gdt_set_entry(0, 0, 0, 0, 0);

    gdt_set_entry(1, 0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE | GDT_ACCESS_RW,
                  GDT_FLAG_GRANULARITY | GDT_FLAG_LONGMODE);

    gdt_set_entry(2, 0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA | GDT_ACCESS_RW,
                  GDT_FLAG_GRANULARITY);

    gdt_set_entry(3, 0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA | GDT_ACCESS_RW,
                  GDT_FLAG_GRANULARITY);

    gdt_set_entry(4, 0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE | GDT_ACCESS_RW,
                  GDT_FLAG_GRANULARITY | GDT_FLAG_LONGMODE);

    __asm__ __volatile__("lgdt %0" : : "m"(gdtr));

    __asm__ __volatile__(
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        "pushq $0x08\n"
        "lea 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:"
        : : : "rax"
    );
}

void gdt_set_tss_entry(int idx, uint64_t base, uint32_t limit)
{
    gdt[idx].limit_low    = (uint16_t)(limit & 0xFFFF);
    gdt[idx].base_low     = (uint16_t)(base & 0xFFFF);
    gdt[idx].base_mid     = (uint8_t)((base >> 16) & 0xFF);
    gdt[idx].access       = 0x89; //present, system, type=9 (available TSS)
    gdt[idx].flags_limit  = (uint8_t)(((limit >> 16) & 0x0F));
    gdt[idx].base_high    = (uint8_t)((base >> 24) & 0xFF);

    gdt[idx+1].limit_low   = (uint16_t)((base >> 32) & 0xFFFF);
    gdt[idx+1].base_low    = (uint16_t)((base >> 48) & 0xFFFF);
    gdt[idx+1].base_mid    = 0;
    gdt[idx+1].access      = 0;
    gdt[idx+1].flags_limit = 0;
    gdt[idx+1].base_high   = 0;
} 