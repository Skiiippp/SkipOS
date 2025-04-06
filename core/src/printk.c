#include "../inc/printk.h"

#include "../inc/assert.h"
#include "../inc/stddef.h"
#include "../inc/vga_display.h"

#include <stdarg.h>
#include <limits.h>

/**
 * BEGIN PRIVATE
 */

 #define OUTBUFFSIZE 64

typedef enum {
    UPPERCASE,
    LOWERCASE
} Case;

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
static void print_hex128(u128 val, Case cs);

static void print_u128_varbase(u128 val, u8 base, Case cs);

// Single digit!
static char int_to_char(u8 val, Case cs);

/**
 * END PRIVATE
 */

int printk(const char *fmt, ...)
{
    va_list args;
    int ret = 0;

    assert(fmt);

    va_start(args, fmt);

    while(*fmt != '\0')
    {
        if(*fmt != '%')
        {
            VGA_display_char(*fmt);
            fmt++;
        }
        else
        {
            fmt++;

            assert(*fmt != '\0');

            switch (*fmt)
            {
                case '%':
                    VGA_display_char('%');
                    break;
                case 'c':
                    print_char((char)va_arg(args, int));
                    break;
                case 's':
                    print_str(va_arg(args, const char *));
                    break;
                case 'p':
                    print_ptr(va_arg(args, const void *));
                    break;
                case 'd':
                    print_s32(va_arg(args, s32));
                    break;
                case 'u':
                    print_u32(va_arg(args, u32));
                    break;
                case 'x':
                    print_hex32(va_arg(args, u32));
                    break;
                case 'h':
                    fmt++;
                    assert(*fmt != '\0');
                    switch(*fmt)
                    {
                        case 'd':
                            print_s16((s16)va_arg(args, int));
                            break;
                        case 'u':
                            print_u16((u16)va_arg(args, unsigned int));
                            break;
                        case 'x':
                            print_hex16((u16)va_arg(args, unsigned int));
                            break;
                        default: assert(0);
                    }
                    break;
                case 'l':
                    fmt++;
                    assert(*fmt != '\0');
                    switch(*fmt)
                    {
                        case 'd':
                            print_s64(va_arg(args, s64));
                            break;
                        case 'u':
                            print_u64(va_arg(args, u64));
                            break;
                        case 'x':
                            print_hex64(va_arg(args, u64));
                            break;
                        default: assert(0);
                    }
                    break;
                case 'q':
                    fmt++;
                    assert(*fmt != '\0');
                    switch(*fmt)
                    {
                        case 'd':
                            print_s128(va_arg(args, s128));
                            break;
                        case 'u':
                            print_u128(va_arg(args, u128));
                            break;
                        case 'x':
                            print_hex128(va_arg(args, u128), LOWERCASE);
                            break;
                        default: assert(0);
                    }
                    break;
                default: assert(0);
            }

            fmt++;
        }
    }

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
    // "Should" be 64 cuz ptr is 64 bits but case issue makes 128 easier
    VGA_display_str("0x"); 
    print_hex128((u64)p, UPPERCASE);
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
    print_hex128(val, LOWERCASE);
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
    print_hex128(val, LOWERCASE);
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
    print_hex128(val, LOWERCASE);
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
    print_u128_varbase(val, 10, LOWERCASE);
}

void print_hex128(u128 val, Case cs)
{
    print_u128_varbase(val, 16, cs);
}

static void print_u128_varbase(u128 val, u8 base, Case cs)
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
            tmp_buff[index] = int_to_char(val % base, cs);
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

char int_to_char(u8 val, Case cs)
{
    char hex_base;

    if(val < 10)
    {
        return ('0' + val);
    }

    if(cs == UPPERCASE)
    {
        hex_base = 'A';
    }
    else
    {
        hex_base = 'a';
    }

    return (hex_base + (val - 0xA));
}