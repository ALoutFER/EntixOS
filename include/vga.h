#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stdarg.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x07

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5
#define VGA_OFFSET_LOW 0x0f
#define VGA_OFFSET_HIGH 0x0e

void kprint(uint8_t *str);
void kprintc(uint8_t *str, uint8_t attr);
void kprinti(int number);
void kprintci(int number, uint8_t attr);
void kprinti_vidmem(int number, int offset);
void kprintci_vidmem(int number, uint8_t attr, int offset);
void putchar(uint8_t character, uint8_t attribute_byte);
void clear_screen();
void write(uint8_t character, uint8_t attribute_byte, uint16_t offset);
void scroll_line();
uint16_t get_cursor();
void set_cursor(uint16_t pos);

uint8_t get_cursor_x();
uint8_t get_cursor_y();

void set_cursor_xy(uint8_t x, uint8_t y);
void disable_cursor();

void kprint_hex(uint32_t value);
void kprint_hex_w(uint32_t value);

void kprintf(const char *fmt, ...);

#endif