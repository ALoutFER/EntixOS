#ifndef PORT_BASED_H
#define PORT_BASED_H

#include <stdint.h>

unsigned char   inb(unsigned short port);
void    outb(unsigned short port, unsigned char data);
unsigned char   inw(unsigned short port);
void outw(unsigned short port, unsigned short data);

#endif // PORT_BASED_H