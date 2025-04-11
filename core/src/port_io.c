#include "../inc/port_io.h"

void wb_port(u16 port, u8 wbyte)
{
    asm volatile ("outb %0, %1" : : "id" (port), "a" (wbyte));
}

u8 rb_port(u16 port)
{
    u8 rbyte;

    asm volatile ("inb %0, %1" : "=a" (rbyte) : "id" (port));

    return rbyte;
}