#include <paging.h>
#include <stdint.h>
#include <stddef.h>

//tables from boot.asm
extern uint8_t pml4_table[];
extern uint8_t pdpt_table[];
extern uint8_t pd_table[];
extern uint8_t pt_table[];

static uint64_t* const pml4 = (uint64_t*)pml4_table;
static uint64_t* const pdpt = (uint64_t*)pdpt_table;
static uint64_t* const pd   = (uint64_t*)pd_table;
static uint64_t* const pt   = (uint64_t*)pt_table;

void paging_init(void) {
    for (uint64_t addr = 0x00000; addr < 0x1000000; addr += PAGE_SIZE) {
        pt[(addr >> 12) & 0x1FF] = addr | PAGE_PRESENT | PAGE_RW;
    }
    //vga!!!!!!!!!!
    pt[(0xB8000 >> 12) & 0x1FF] = 0xB8000 | PAGE_PRESENT | PAGE_RW;
    pt[((uint64_t)pml4 >> 12) & 0x1FF] = (uint64_t)pml4 | PAGE_PRESENT | PAGE_RW;
    pt[((uint64_t)pdpt >> 12) & 0x1FF] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_RW;
    pt[((uint64_t)pd >> 12) & 0x1FF]   = (uint64_t)pd   | PAGE_PRESENT | PAGE_RW;
    pt[((uint64_t)pt >> 12) & 0x1FF]   = (uint64_t)pt   | PAGE_PRESENT | PAGE_RW;
}

void paging_map_page(uint64_t virt_addr, uint64_t phys_addr, uint64_t flags) {
    pt[(virt_addr >> 12) & 0x1FF] = (phys_addr & ~0xFFF) | (flags & 0xFFF);
}