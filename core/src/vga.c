#include "../inc/vga.h"

#include "../inc/string.h"
#include "../inc/stddef.h"
#include "../inc/assert.h"

#define VGA_BASE 0xb8000

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CELL_COUNT (VGA_WIDTH * VGA_HEIGHT)

#define LIGHT_GREY 0x7
#define BLACK 0x0
#define VGA_COLOR ((BLACK << 4) | LIGHT_GREY)
#define VGA_COLOR_MASK (VGA_COLOR << 8)

static u16 *vga_buff = (u16 *)VGA_BASE;

static u16 x_pos = 0, y_pos = 0;
static u16 cursor = 0;

/**
 * BEGIN PRIVATE
 */

static void write_char(char c);
static void set_cursor();
static void scroll();
static void clear_bottom_row();

/**
 * END PRIVATE
 */

void VGA_clear()
{
    memset(vga_buff, VGA_COLOR_MASK, VGA_WIDTH * VGA_HEIGHT * sizeof(u16));
}

void VGA_display_char(char c)
{
    assert(x_pos < VGA_WIDTH && y_pos < VGA_HEIGHT && cursor < VGA_CELL_COUNT);

    if(c == '\n')
    {
        y_pos += 1;
        x_pos = 0;
    }
    else if (c == '\r')
    {
        y_pos += 1;
    }
    else
    {
        write_char(c);
        x_pos += 1;
    }

    assert(x_pos <= VGA_WIDTH && y_pos <= VGA_HEIGHT);

    if(x_pos == VGA_WIDTH)
    {
        y_pos += 1;
        x_pos = 0;
    }

    if(y_pos == VGA_HEIGHT)
    {
        scroll();
        y_pos -= 1;
    }

    set_cursor();

    assert(x_pos < VGA_WIDTH && y_pos < VGA_HEIGHT && cursor < VGA_CELL_COUNT);
}

void VGA_display_str(const char *s)
{
    u16 index = 0;

    assert(s);

    while(s[index] != '\0')
    {
        VGA_display_char(s[index]);
        index += 1;
    }
}

void write_char(char c)
{
    vga_buff[cursor] = c | VGA_COLOR_MASK; 
}

void set_cursor()
{
    assert(cursor < VGA_CELL_COUNT);
    cursor = x_pos + y_pos * VGA_WIDTH;
}

void scroll()
{
    u16 temp_vga_buff[VGA_CELL_COUNT];

    memcpy(temp_vga_buff, vga_buff, VGA_CELL_COUNT * sizeof(u16));
    memcpy(vga_buff, temp_vga_buff + VGA_WIDTH, (VGA_CELL_COUNT - VGA_WIDTH) * sizeof(u16));
    clear_bottom_row();
}

void clear_bottom_row()
{
    memset(vga_buff + VGA_CELL_COUNT - VGA_WIDTH, VGA_COLOR_MASK, VGA_WIDTH * sizeof(u16));
}