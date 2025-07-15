#include <string.h>
#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void memset(void *s, int c, size_t n)
{
    char *p = s;
    while (n--)
        *p++ = c;
}