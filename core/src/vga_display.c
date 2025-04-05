#include "../inc/vga_display.h"

#define VGA_BASE 0xb80000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static unsigned short *vga_buff = (unsigned short *)VGA_BASE;

void VGA_clear()
{

}

void VGA_display_char(char c)
{

}

void VGA_display_str(const char *s)
{

}