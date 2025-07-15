section .multiboot2 align=8
    dd 0xe85250d6                ; magic
    dd 0                         ; architecture (0 = i386+)
    dd header_end - $$           ; header length
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - $$)) ; checksum

    ; End tag
    dw 0
    dw 0
    dd 8
header_end:

section .bss
align 16
global stack_bottom
stack_bottom:
    resb 16384 ;16KB stack
global stack_top
stack_top:
align 4096
global pml4_table
pml4_table: resb 4096
global pdpt_table
pdpt_table: resb 4096
global pd_table
pd_table:  resb 4096
global pt_table
pt_table:  resb 4096

section .text
bits 32
global _start
extern kernel_main

_start:
    mov esp, stack_top
    push ebx                     ;multiboot2 pointer
    push eax                   ;magic
    call check_cpuid
    call check_long_mode
    pop eax
    pop ebx
    mov edi, eax
    mov esi, ebx
    call setup_page_tables
    call enable_paging
    lea eax, [tmp_gdt_ptr]
    lgdt [eax]
    jmp 0x08:long_mode_start
    hlt

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, '1'
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, '2'
    jmp error

setup_page_tables:
    mov eax, page_table_l3
    or  eax, 0b11
    mov [page_table_l4], eax
    mov ecx, 0
.map_l3_table:
    mov eax, ecx
    shl eax, 30
    or  eax, 0b10000011
    mov [page_table_l3 + ecx*8], eax
    inc ecx
    cmp ecx, 4
    jne .map_l3_table
    ret

enable_paging:
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov eax, page_table_l4
    mov cr3, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret

error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

section .bss
align 4096
page_table_l4:
    resb 4096
page_table_l3:
    resb 4096

section .rodata
align 8
tmp_gdt:
    dq 0x0000000000000000
    dq 0x00AF9A000000FFFF   ; 0x08: 64-bit code segment
    dq 0x00AF92000000FFFF   ; 0x10: 64-bit data segment
tmp_gdt_end:
tmp_gdt_ptr:
    dw tmp_gdt_end - tmp_gdt - 1
    dq tmp_gdt

section .text
bits 64
long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, stack_top
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang 