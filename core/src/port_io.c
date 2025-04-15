#include "../inc/port_io.h"

/**
 * BEGIN PRIVATE
 */

#define UNUSED_PORT 0x80

/**
 * END PRIVATE
 */

void outb(u16 port, u8 val)
{
    asm volatile ("outb %0, %1" : : "id" (port), "a" (val));
}

u8 inb(u16 port)
{
    u8 val;

    asm volatile ("inb %0, %1" : "=a" (val) : "id" (port));

    return val;
}

void io_wait()
{
    outb(UNUSED_PORT, 0);
}