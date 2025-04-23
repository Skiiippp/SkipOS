#include "../inc/vga.h"

#include "../inc/string.h"
#include "../inc/common.h"
#include "../inc/assert.h"
#include "../inc/interrupt.h"

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

// UNUSED
//static void clear_at_coord(u16 x, u16 y);

static void clear_at_offset(u16 offset);

// Offset into VGA buff
static u16 get_offset_from_coord(u16 x, u16 y);

static void clear_bottom_row();

/**
 * END PRIVATE
 */

void VGA_clear()
{
    for (u16 offset = 0; offset < VGA_CELL_COUNT; offset++)
    {
        clear_at_offset(offset);
    }
}

void VGA_display_char(char c)
{
    NESTED_SAFE_CLI;

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

    NESTED_SAFE_STI;
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

void VGA_backspace_char()
{
    if (x_pos != 0)
    {
        x_pos -= 1;
        set_cursor();
        vga_buff[cursor] = VGA_COLOR_MASK;
    }
}

void write_char(char c)
{
    vga_buff[cursor] = c | VGA_COLOR_MASK; 
}

void set_cursor()
{
    assert(cursor < VGA_CELL_COUNT);
    cursor = get_offset_from_coord(x_pos, y_pos);
}

void scroll()
{
    u16 temp_vga_buff[VGA_CELL_COUNT];

    memcpy(temp_vga_buff, vga_buff, VGA_CELL_COUNT * sizeof(u16));
    memcpy(vga_buff, temp_vga_buff + VGA_WIDTH, (VGA_CELL_COUNT - VGA_WIDTH) * sizeof(u16));
    clear_bottom_row();
}

u16 get_offset_from_coord(u16 x, u16 y)
{
    return x + y * VGA_WIDTH;
}

/* UNUSED
void clear_at_coord(u16 x, u16 y)
{
    clear_at_offset(get_offset_from_coord(x, y));
}
*/

void clear_at_offset(u16 offset)
{
    assert(offset < VGA_CELL_COUNT);
    vga_buff[offset] = VGA_COLOR_MASK;
}

void clear_bottom_row()
{
    for (u16 offset = get_offset_from_coord(0, VGA_HEIGHT - 1); offset < VGA_CELL_COUNT; offset++)
    {
        clear_at_offset(offset);
    }
}