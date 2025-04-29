#include "../inc/pic.h"

#include "../inc/port_io.h"


/**
 * BEGIN PRIVATE
 */

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

// From OSDev Wiki (Next 10)
#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define EOI_CMD 0x20

/**
 * END PRIVATE
 */

void PIC_disable_all_pic_irqs()
{
    PIC_set_pic1_mask(0xFF);
    PIC_set_pic2_mask(0xFF);
}

// From OSDev Wiki
void PIC_remap(u8 base)
{
    const u8 pic1_offset = base;
    const u8 pic2_offset = base + 8;

    outb(PIC1_CMD_PORT, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_CMD_PORT, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA_PORT, pic1_offset);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA_PORT, pic2_offset);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA_PORT, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA_PORT, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
	
	outb(PIC1_DATA_PORT, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA_PORT, ICW4_8086);
	io_wait();

	// Unmask both PICs.
	outb(PIC1_DATA_PORT, 0);
	outb(PIC2_DATA_PORT, 0);
}

void PIC_set_pic1_mask(u8 mask)
{
    outb(PIC1_DATA_PORT, mask);
}

void PIC_set_pic2_mask(u8 mask)
{
    outb(PIC2_DATA_PORT, mask);
}

u8 PIC_get_pic1_mask()
{
    return inb(PIC1_DATA_PORT);
}

u8 PIC_get_pic2_mask()
{
    return inb(PIC2_DATA_PORT);
}

void PIC_send_pic1_eoi()
{
    outb(PIC1_CMD_PORT, EOI_CMD);
}

void PIC_send_pic2_eoi()
{
    outb(PIC2_CMD_PORT, EOI_CMD);
}
