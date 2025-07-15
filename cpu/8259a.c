#include <pic.h>
#include <port_based.h>

#define ICW1_ICW4   0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL  0x08
#define ICW1_INIT   0x10

#define ICW4_8086   0x01
#define ICW4_AUTO   0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM   0x10

void pic_remap(int offset1, int offset2)
{
    //save masks
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    //set up new vecs
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    //set up cascade
    outb(PIC1_DATA, 4); //tell master slave on irq2
    outb(PIC2_DATA, 2); //tell slave cascade number

    //8086 mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    //returning masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_set_mask(uint8_t irq_line)
{
    uint16_t port = (irq_line < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t value = inb(port) | (1 << (irq_line & 7));
    outb(port, value);
}

void pic_clear_mask(uint8_t irq_line)
{
    uint16_t port = (irq_line < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t value = inb(port) & ~(1 << (irq_line & 7));
    outb(port, value);
} 