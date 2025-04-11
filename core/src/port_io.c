#include "../inc/port_io.h"

inline void outb(u16 port, u8 val)
{
    asm volatile ("outb %0, %1" : : "id" (port), "a" (val));
}

u8 inb(u16 port)
{
    u8 val;

    asm volatile ("inb %0, %1" : "=a" (val) : "id" (port));

    return val;
}