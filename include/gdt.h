#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  flags_limit;
    uint8_t  base_high;
} gdt_entry_t;

typedef struct __attribute__((packed, aligned(1))) {
    uint16_t size;
    uint64_t offset;
} gdt_descriptor_t;

void gdt_init(void);

void gdt_set_tss_entry(int idx, uint64_t base, uint32_t limit);

#endif 