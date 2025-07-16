#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <vga.h>

#define KINFO 1
#define KWARN 2
#define KERR 3
#define KPANIC 4

void kdbg(int lvl, char *fmt, ...);

#endif