#include <ps2.h>
#include <cpu.h>
#include <port_based.h>
#include <idt.h>
#include <vga.h>

#define KB_BUF_SIZE 128
static char kb_buf[KB_BUF_SIZE];
static volatile uint32_t kb_head = 0;
static volatile uint32_t kb_tail = 0;

static const char scancode_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,
    ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const char scancode_ascii_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0,
    '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0,
    ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static int shift_pressed = 0;
static int caps_lock = 0;

void keyboard_buffer_push(char c)
{
    uint32_t next = (kb_head + 1) % KB_BUF_SIZE;
    if (next != kb_tail) {
        kb_buf[kb_head] = c;
        kb_head = next;
    }
}

int keyboard_buffer_pop(char *c)
{
    if (kb_head == kb_tail) {
        return 0;
    }
    *c = kb_buf[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUF_SIZE;
    return 1;
}

static int key_end = 0;

static void keyboard_handler(cpu_registers_t* regs)
{
    unsigned char scancode = inb(0x60);
    if (scancode == 42 || scancode == 54) {
        shift_pressed = 1;
        return;
    }
    if (scancode == (42 | 0x80) || scancode == (54 | 0x80)) {
        shift_pressed = 0;
        return;
    }
    if (scancode == 58) {
        caps_lock = !caps_lock;
        return;
    }
    if (scancode < 128) {
        char c = scancode_ascii[scancode];
        char c_shift = scancode_ascii_shift[scancode];
        if (c >= 'a' && c <= 'z') {
            if (shift_pressed ^ caps_lock) {
                c = c - 'a' + 'A';
            }
        } else if (c >= 'A' && c <= 'Z') {
            if (!(shift_pressed ^ caps_lock)) {
                c = c - 'A' + 'a';
            }
        } else if (shift_pressed && c_shift) {
            c = c_shift;
        }
        if (c) {
            keyboard_buffer_push(c);
        }
    }
}

void ps2_init(void) {
    idt_register_handler(33, keyboard_handler);
}

char kgetch(void)
{
    char c;
    idt_register_handler(33, keyboard_handler);
    while (!keyboard_buffer_pop(&c));
    key_end = 0;
    return c;
}

char *kgets()
{
    static char buf[256];
    int pos = 0;
    while (1) {
        char c = kgetch();
        if (c == '\n' || c == '\r') {
            kprintf("\n");
            buf[pos] = '\0';
            break;
        } else if ((c == '\b' || c == 127) && pos > 0) {
            pos--;
            kprintf("\b \b");
        } else if (c >= 32 && c < 127 && pos < 255) {
            buf[pos++] = c;
            kprintf("%c", c);
        }
    }
    return buf;
}