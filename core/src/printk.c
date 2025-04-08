#include "../inc/printk.h"

#include "../inc/assert.h"
#include "../inc/common.h"
#include "../inc/vga.h"

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

static void print_short(short val);
static void print_unsigned_short(unsigned short val);
static void print_hex_short(unsigned short val);

static void print_int(int val);
static void print_unsigned_int(unsigned int val);
static void print_hex_int(unsigned int val);

static void print_long(long val);
static void print_unsigned_long(unsigned long val);
static void print_hex_long(unsigned long val);

static void print_long_long(long long val);
static void print_unsigned_long_long(unsigned long long val);
static void print_hex_long_long(unsigned long long val, Case cs);

static void print_unsigned_long_long_varbase(unsigned long long val, u8 base, Case cs);

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
                    print_int(va_arg(args, int));
                    break;
                case 'u':
                    print_unsigned_int(va_arg(args, unsigned int));
                    break;
                case 'x':
                    print_hex_int(va_arg(args, unsigned int));
                    break;
                case 'h':
                    fmt++;
                    assert(*fmt != '\0');
                    switch(*fmt)
                    {
                        case 'd':
                            print_short((short)va_arg(args, int));
                            break;
                        case 'u':
                            print_unsigned_short((unsigned short)va_arg(args, unsigned int));
                            break;
                        case 'x':
                            print_hex_short((unsigned short)va_arg(args, unsigned int));
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
                            print_long(va_arg(args, long));
                            break;
                        case 'u':
                            print_unsigned_long(va_arg(args, unsigned long));
                            break;
                        case 'x':
                            print_hex_long(va_arg(args, unsigned long));
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
                            print_long_long(va_arg(args, long long));
                            break;
                        case 'u':
                            print_unsigned_long_long(va_arg(args, unsigned long long));
                            break;
                        case 'x':
                            print_hex_long_long(va_arg(args, unsigned long long), LOWERCASE);
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
    print_hex_long_long((unsigned long)p, UPPERCASE);
}

void print_short(short val)
{
    print_long_long(val);
}

void print_unsigned_short(unsigned short val)
{
    print_unsigned_long_long(val);
}

void print_hex_short(unsigned short val)
{
    print_hex_long_long(val, LOWERCASE);
}

void print_int(int val)
{
    print_long_long(val);
}

void print_unsigned_int(unsigned int val)
{
    print_unsigned_long_long(val);
}

void print_hex_int(unsigned int val)
{
    print_hex_long_long(val, LOWERCASE);
}

void print_long(long val)
{
    print_long_long(val);
}

void print_unsigned_long(unsigned long val)
{
    print_unsigned_long_long(val);
}

void print_hex_long(unsigned long val)
{
    print_hex_long_long(val, LOWERCASE);
}

void print_long_long(long long val)
{
    if(val < 0)
    {
        VGA_display_char('-');
        print_unsigned_long_long((unsigned long long)(val * -1));
    }
    else
    {
        print_unsigned_long_long((unsigned long long)val);
    }
}

void print_unsigned_long_long(unsigned long long val)
{
    print_unsigned_long_long_varbase(val, 10, LOWERCASE);
}

void print_hex_long_long(unsigned long long val, Case cs)
{
    print_unsigned_long_long_varbase(val, 16, cs);
}

static void print_unsigned_long_long_varbase(unsigned long long val, u8 base, Case cs)
{
    char tmp_buff[OUTBUFFSIZE];
    char index = 0;

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