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

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}