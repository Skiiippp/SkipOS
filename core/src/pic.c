#include "../inc/pic.h"

#include "../inc/port_io.h"


/**
 * BEGIN PRIVATE
 */

#define PIC1_CMD_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_CMD_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

/**
 * END PRIVATE
 */

void disable_pic()
{
    outb(PIC1_DATA_PORT, 0xFF);
    outb(PIC2_DATA_PORT, 0xFF);
}