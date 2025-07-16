#include <vga.h>
#include <stdarg.h>
#include "debug.h"

void kdbg(int lvl, char *fmt, ...) {
    switch (lvl) {
        case 1:
            kprintf("<(0F)>[INFO]:<(07)> ");
            break;
        case 2:
            kprintf("<(0E)>[WARN]:<(07)> ");
            break;
        case 3:
            kprintf("<(0C)>[ERR]:<(07)> ");
            break;
        case 4:
            kprintf("<(03)>[PANIC]:<(07)> ");
            break;
        default:
            kprintf("<(08)>[DBG]:<(07)> ");
            break;
    }
    va_list args;
    va_start(args, fmt);
    kvprintf(fmt, args);
    va_end(args);
}
