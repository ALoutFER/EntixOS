#include <vga.h>
#include <port_based.h>
#include <stdint.h>
#include <stdarg.h>

static void	vga_memcpy(uint8_t *src, uint8_t *dest, int bytes);
static void	vga_memcpy(uint8_t *src, uint8_t *dest, int bytes)
{
	int i;

	i = 0;
	while (i < bytes)
	{
		dest[i] = src[i];
		i++;
	}
}

void	kprint(uint8_t *str)
{
    uint8_t color = WHITE_ON_BLACK;
    while (*str) {
        if (*str == '<' && *(str+1) == '(' &&
            ((*(str+2) >= '0' && *(str+2) <= '9') || (*(str+2) >= 'A' && *(str+2) <= 'F')) &&
            ((*(str+3) >= '0' && *(str+3) <= '9') || (*(str+3) >= 'A' && *(str+3) <= 'F')) &&
            *(str+4) == ')' && *(str+5) == '>') {
            uint8_t hi = (*(str+2) <= '9') ? *(str+2) - '0' : (*(str+2) - 'A' + 10);
            uint8_t lo = (*(str+3) <= '9') ? *(str+3) - '0' : (*(str+3) - 'A' + 10);
            color = (hi << 4) | lo;
            str += 6;
            continue;
        }
        putchar(*str, color);
        str++;
    }
}

void	putchar(uint8_t character, uint8_t attribute_byte)
{
	uint16_t offset;
	offset = get_cursor();
	if (character == '\n')
	{
		if ((offset / 2 / MAX_COLS) == (MAX_ROWS - 1)) 
			scroll_line();
		else
			set_cursor((offset - offset % (MAX_COLS*2)) + MAX_COLS*2);
	}
    else if (character == '\b')
    {
        set_cursor(get_cursor() - 1);
        putchar(' ', attribute_byte);
        set_cursor(get_cursor() - 2);
    }
	else 
	{
		if (offset == (MAX_COLS * MAX_ROWS * 2)) scroll_line();
		write(character, attribute_byte, offset);
		set_cursor(offset+2);
	}
}

void	scroll_line()
{
    for (uint8_t i = 1; i < MAX_ROWS; i++) {
        vga_memcpy((uint8_t *)(VIDEO_ADDRESS + (MAX_COLS * i * 2)),
                   (uint8_t *)(VIDEO_ADDRESS + (MAX_COLS * (i - 1) * 2)),
                   (MAX_COLS * 2));
    }
    uint16_t last_line = (MAX_ROWS - 1) * MAX_COLS * 2;
    for (uint8_t i = 0; i < MAX_COLS; i++) {
        write(' ', WHITE_ON_BLACK, last_line + i * 2);
    }
    set_cursor(last_line);
}

void	clear_screen()
{
	uint16_t	offset = 0;
	while (offset < (MAX_ROWS * MAX_COLS * 2))
	{
		write('\0', WHITE_ON_BLACK, offset);
		offset += 2;
	}
	set_cursor(0);
}

void	write(uint8_t character, uint8_t attribute_byte, uint16_t offset)
{
	uint8_t *vga = (uint8_t *) VIDEO_ADDRESS;
	vga[offset] = character;
	vga[offset + 1] = attribute_byte;
}

uint16_t		get_cursor()
{
	outb(REG_SCREEN_CTRL, 14);
	uint8_t high_byte = inb(REG_SCREEN_DATA);
	outb(REG_SCREEN_CTRL, 15);
	uint8_t low_byte = inb(REG_SCREEN_DATA);
	return (((high_byte << 8) + low_byte) * 2);
}

void	set_cursor(uint16_t pos)
{
	pos /= 2;

	outb(REG_SCREEN_CTRL, 14);
	outb(REG_SCREEN_DATA, (uint8_t)(pos >> 8));
	outb(REG_SCREEN_CTRL, 15);
	outb(REG_SCREEN_DATA, (uint8_t)(pos & 0xff));
}

uint8_t get_cursor_x() {
    uint16_t pos = get_cursor();
    return pos % 80;
}

uint8_t get_cursor_y() {
    uint16_t pos = get_cursor();
    return pos / 80;
}

void set_cursor_xy(uint8_t x, uint8_t y) {
    uint16_t pos;
    
    if (x >= MAX_ROWS) x = MAX_ROWS - 1;
    if (y >= MAX_COLS) y = MAX_COLS - 1;
    
    pos = y * MAX_ROWS + x;
    set_cursor(pos);
}

void disable_cursor()
{
	outb(REG_SCREEN_CTRL, 0x0A);
	outb(REG_SCREEN_DATA, 0x20);
}
void kprint_hex(uint32_t value) {

    char hex_str[9];
    hex_str[8] = '\0';

    for (int i = 7; i >= 0; i--) {
        uint8_t digit = value & 0xF;
        if (digit < 10) {
            hex_str[i] = '0' + digit;
        } else {
            hex_str[i] = 'A' + (digit - 10);
        }
        value >>= 4;
    }

    for (int i = 0; i < 8; i++) {
        putchar(hex_str[i],  0x07);
    }
}
void kprintc(uint8_t *str, uint8_t attr)
{
    while (*str)
	{
		if (*str == '\b')
		{
			putchar(' ', attr);
			set_cursor(get_cursor() - 2);
		}
		else {
			putchar(*str, attr);
            set_cursor(get_cursor()+1);
		}
		str++;
	}
}
void int_to_str(int num, char *str);
void int_to_str(int num, char *str) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void kprinti(int number) {
    char buffer[12];
    int_to_str(number, buffer);
	kprint(buffer);
}

void kprintci(int number, uint8_t attr) {
    char buffer[12];
	int_to_str(number, buffer);
    kprintc(buffer, attr);
}

void kprinti_vidmem(int number, int offset) {
    char buffer[12];
    int_to_str(number, buffer);
    volatile char *video = (volatile char *)0xB8000;
    for (int i = 0; buffer[i] != '\0'; i++) {
        video[offset + i * 2] = buffer[i];
        video[offset + i * 2 + 1] = 0x07;
    }
}

void kprintci_vidmem(int number, uint8_t attr, int offset) {
    
}
void kprint_hex_w(uint32_t value) {
    char hex_str[5];
    hex_str[4] = '\0';

    for (int i = 3; i >= 0; i--) {
        uint8_t digit = value & 0xF;
        if (digit < 10) {
            hex_str[i] = '0' + digit;
        } else {
            hex_str[i] = 'A' + (digit - 10);
        }
        value >>= 4;
    }

    for (int i = 0; i < 4; i++) {
        putchar(hex_str[i], 0x07);
        set_cursor(get_cursor()+2);
    }
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[256];
    int len = 0;
    const char *f = fmt;
    while (*f && len < 255) {
        if (*f != '%') {
            buf[len++] = *f++;
            continue;
        }
        f++;
        int width = 0, zero_pad = 0;
        if (*f == '0') { zero_pad = 1; f++; }
        while (*f >= '0' && *f <= '9') { width = width * 10 + (*f++ - '0'); }
        switch (*f) {
            case 'd': {
                int val = va_arg(args, int);
                int is_neg = val < 0;
                unsigned int uval = is_neg ? -val : val;
                int i = 0;
                char numbuf[32];
                do { numbuf[i++] = '0' + (uval % 10); uval /= 10; } while (uval);
                if (is_neg) numbuf[i++] = '-';
                while (i < width) numbuf[i++] = zero_pad ? '0' : ' ';
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'u': {
                unsigned int val = va_arg(args, unsigned int);
                int i = 0;
                char numbuf[32];
                do { numbuf[i++] = '0' + (val % 10); val /= 10; } while (val);
                while (i < width) numbuf[i++] = zero_pad ? '0' : ' ';
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'x': case 'X': {
                unsigned int val = va_arg(args, unsigned int);
                int i = 0;
                char numbuf[32];
                do {
                    int digit = val & 0xF;
                    numbuf[i++] = (digit < 10) ? ('0' + digit) : ((*f == 'x' ? 'a' : 'A') + digit - 10);
                    val >>= 4;
                } while (val);
                while (i < width) numbuf[i++] = zero_pad ? '0' : ' ';
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'p': {
                unsigned long val = (unsigned long)va_arg(args, void*);
                buf[len++] = '0'; buf[len++] = 'x';
                int i = 0;
                char numbuf[32];
                do {
                    int digit = val & 0xF;
                    numbuf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                    val >>= 4;
                } while (val);
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'c': {
                char ch = (char)va_arg(args, int);
                buf[len++] = ch;
                break;
            }
            case 's': {
                char *s = va_arg(args, char*);
                int slen = 0;
                while (s[slen]) slen++;
                int pad = width - slen;
                while (pad-- > 0) buf[len++] = zero_pad ? '0' : ' ';
                while (*s) buf[len++] = *s++;
                break;
            }
            case '%': {
                buf[len++] = '%';
                break;
            }
            default:
                buf[len++] = '%';
                buf[len++] = *f;
                break;
        }
        f++;
    }
    buf[len] = 0;
    va_end(args);
    kprint((uint8_t*)buf);
}

void kvprintf(const char *fmt, va_list args) {
    char buf[256];
    int len = 0;
    const char *f = fmt;
    while (*f && len < 255) {
        if (*f != '%') {
            buf[len++] = *f++;
            continue;
        }
        f++;
        int width = 0, zero_pad = 0;
        if (*f == '0') { zero_pad = 1; f++; }
        while (*f >= '0' && *f <= '9') { width = width * 10 + (*f++ - '0'); }
        switch (*f) {
            case 'd': {
                int val = va_arg(args, int);
                int is_neg = val < 0;
                unsigned int uval = is_neg ? -val : val;
                int i = 0;
                char numbuf[32];
                do { numbuf[i++] = '0' + (uval % 10); uval /= 10; } while (uval);
                if (is_neg) numbuf[i++] = '-';
                while (i < width) numbuf[i++] = zero_pad ? '0' : ' ';
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'u': {
                unsigned int val = va_arg(args, unsigned int);
                int i = 0;
                char numbuf[32];
                do { numbuf[i++] = '0' + (val % 10); val /= 10; } while (val);
                while (i < width) numbuf[i++] = zero_pad ? '0' : ' ';
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'x': case 'X': {
                unsigned int val = va_arg(args, unsigned int);
                int i = 0;
                char numbuf[32];
                do {
                    int digit = val & 0xF;
                    numbuf[i++] = (digit < 10) ? ('0' + digit) : ((*f == 'x' ? 'a' : 'A') + digit - 10);
                    val >>= 4;
                } while (val);
                while (i < width) numbuf[i++] = zero_pad ? '0' : ' ';
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'p': {
                unsigned long val = (unsigned long)va_arg(args, void*);
                buf[len++] = '0'; buf[len++] = 'x';
                int i = 0;
                char numbuf[32];
                do {
                    int digit = val & 0xF;
                    numbuf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                    val >>= 4;
                } while (val);
                while (i--) buf[len++] = numbuf[i];
                break;
            }
            case 'c': {
                char ch = (char)va_arg(args, int);
                buf[len++] = ch;
                break;
            }
            case 's': {
                char *s = va_arg(args, char*);
                int slen = 0;
                while (s[slen]) slen++;
                int pad = width - slen;
                while (pad-- > 0) buf[len++] = zero_pad ? '0' : ' ';
                while (*s) buf[len++] = *s++;
                break;
            }
            case '%': {
                buf[len++] = '%';
                break;
            }
            default:
                buf[len++] = '%';
                buf[len++] = *f;
                break;
        }
        f++;
    }
    buf[len] = 0;
    kprint((uint8_t*)buf);
}