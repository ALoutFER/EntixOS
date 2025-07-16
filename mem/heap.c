#include "heap.h"
#include <stddef.h>
#include <stdint.h>
#define ALIGN4(x) (((((x)-1)>>2)<<2)+4)
typedef struct heap_block {
    size_t size;
    int free;
    struct heap_block *next;
    struct heap_block *prev;
} heap_block_t;
#define BLOCK_SIZE ALIGN4(sizeof(heap_block_t))
static heap_block_t *heap_head = 0;
static uint8_t *heap_base = 0;
static size_t heap_total_size = 0;
void heap_init(uint64_t heap_start, uint64_t heap_size) {
    heap_base = (uint8_t*)heap_start;
    heap_total_size = heap_size;
    heap_head = (heap_block_t*)heap_base;
    heap_head->size = heap_total_size - BLOCK_SIZE;
    heap_head->free = 1;
    heap_head->next = NULL;
    heap_head->prev = NULL;
}
static void split_block(heap_block_t *block, size_t size) {
    if (block->size > size + BLOCK_SIZE) {
        heap_block_t *new_block = (heap_block_t*)((uint8_t*)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        if (block->next) block->next->prev = new_block;
        block->size = size;
        block->next = new_block;
    }
}
void *kmalloc(size_t size) {
    size = ALIGN4(size);
    heap_block_t *curr = heap_head;
    while (curr) {
        if (curr->free && curr->size >= size) {
            split_block(curr, size);
            curr->free = 0;
            return (void*)((uint8_t*)curr + BLOCK_SIZE);
        }
        curr = curr->next;
    }
    return NULL;
}
void kfree(void *ptr) {
    if (!ptr) return;
    heap_block_t *block = (heap_block_t*)((uint8_t*)ptr - BLOCK_SIZE);
    block->free = 1;
    if (block->next && block->next->free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
        if (block->next) block->next->prev = block;
    }
    if (block->prev && block->prev->free) {
        block->prev->size += BLOCK_SIZE + block->size;
        block->prev->next = block->next;
        if (block->next) block->next->prev = block->prev;
    }
}
void *krealloc(void *ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    heap_block_t *block = (heap_block_t*)((uint8_t*)ptr - BLOCK_SIZE);
    if (block->size >= size) return ptr;
    void *newptr = kmalloc(size);
    if (!newptr) return NULL;
    for (size_t i = 0; i < block->size; ++i)
        ((uint8_t*)newptr)[i] = ((uint8_t*)ptr)[i];
    kfree(ptr);
    return newptr;
}
void *kcalloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = kmalloc(total);
    if (!ptr) return NULL;
    for (size_t i = 0; i < total; ++i)
        ((uint8_t*)ptr)[i] = 0;
    return ptr;
}
size_t heap_total(void) { return heap_total_size; }
size_t heap_used(void) {
    size_t used = 0;
    heap_block_t *curr = heap_head;
    while (curr) {
        if (!curr->free) used += curr->size;
        curr = curr->next;
    }
    return used;
}
size_t heap_free(void) {
    size_t free = 0;
    heap_block_t *curr = heap_head;
    while (curr) {
        if (curr->free) free += curr->size;
        curr = curr->next;
    }
    return free;
} 