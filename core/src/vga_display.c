#include "../inc/vga_display.h"

#include "../inc/string.h"
#include "../inc/stddef.h"

#define VGA_BASE 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t *vga_buff = (uint16_t *)VGA_BASE;

void VGA_clear()
{
    memset(vga_buff, 0, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
}

void VGA_display_char(char c)
{

}

void VGA_display_str(const char *s)
{

}