#include "../inc/printk.h"

#include "../inc/assert.h"
#include "../inc/stddef.h"
#include "../inc/vga_display.h"

#include <stdarg.h>
#include <limits.h>

#define OUTBUFFSIZE 64

/**
 * BEGIN PRIVATE
 */


static void print_char(char c);
static void print_str(const char *s);
static void print_ptr(const void *p);

static void print_s16(s16 val);
static void print_u16(u16 val);
static void print_hex16(u16 val);

static void print_s32(s32 val);
static void print_u32(u32 val);
static void print_hex32(u32 val);

static void print_s64(s64 val);
static void print_u64(u64 val);
static void print_hex64(u64 val);

static void print_s128(s128 val);
static void print_u128(u128 val);
static void print_hex128(u128 val);

static void print_u128_varbase(u128 val, u8 base);

// Single digit!
static char int_to_char(u8 val);

/**
 * END PRIVATE
 */

void test()
{
    print_hex32(0xDEADBEEF);
}

int printk(const char *fmt, ...)
{
    va_list args;
    int ret = 0;

    va_start(args, fmt);

    va_end(args);

    return ret;
}

void print_char(char c)
{
    VGA_display_char(c);
}

void print_str(const char *s)
{
    VGA_display_str(s);
}

void print_ptr(const void *p)
{
    //print_hex64((u64)p);
}

void print_s16(s16 val)
{
    print_s128(val);
}

void print_u16(u16 val)
{
    print_u128(val);
}

void print_hex16(u16 val)
{
    print_hex128(val);
}

void print_s32(s32 val)
{
    print_s128(val);
}

void print_u32(u32 val)
{
    print_u128(val);
}

void print_hex32(u32 val)
{
    print_hex128(val);
}

void print_s64(s64 val)
{
    print_s128(val);
}

void print_u64(u64 val)
{
    print_u128(val);
}

void print_hex64(u64 val)
{
    print_hex128(val);
}

void print_s128(s128 val)
{
    if(val < 0)
    {
        VGA_display_char('-');
        print_u128((u128)(val * -1));
    }
    else
    {
        print_u128((u128)val);
    }
}

void print_u128(u128 val)
{
    print_u128_varbase(val, 10);
}

void print_hex128(u128 val)
{
    print_u128_varbase(val, 16);
}

static void print_u128_varbase(u128 val, u8 base)
{
    char tmp_buff[OUTBUFFSIZE];
    s8 index = 0;

    if(val == 0)
    {
        VGA_display_char('0');
    }
    else
    {
        while(val > 0)
        {
            tmp_buff[index] = int_to_char(val % base);
            val /= base;
            index += 1;
        }

        index -= 1;

        while(index >= 0)
        {
            VGA_display_char(tmp_buff[index]);
            index -= 1;
        }
    }
}

char int_to_char(u8 val)
{
    if(val < 10)
    {
        return ('0' + val);
    }

    return ('a' + (val - 0xA));
}