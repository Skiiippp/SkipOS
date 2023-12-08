#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>

#include "print.h"
#include "../include/types.h"
#include "../drivers/uart/uart.h"

int toupper(int c) {
	return 'a' <= c && c <= 'z' ? c + 'A' - 'a' : c;
}

int kputchar(int character) {
	uart_put(UART_ADDR, (u8)character);
	return character;
}

static void kprint(const char *str) {
	while (*str) {
		kputchar((int)*str);
		++str;
	}
}

int kputs(const char *str) {
	kprint(str);
	kputchar((int)'\n');
	return 0;
}

// Limited version of vprintf() which only supports the following specifiers:
//
// - d/i: Signed decimal integer
// - u: Unsigned decimal integer
// - o: Unsigned octal
// - x: Unsigned hexadecimal integer
// - X: Unsigned hexadecimal integer (uppercase)
// - c: Character
// - s: String of characters
// - p: Pointer address
// - %: Literal '%'
//
// None of the sub-specifiers are supported for the sake of simplicity.
// The `n` specifier is not supported since that is a major source of
// security vulnerabilities. None of the floating-point specifiers are
// supported since floating point operations don't make sense in kernel
// space
//
// Anyway, this subset should suffice for printf debugging
void kvprintf(const char *format, va_list arg) {
	while (*format) {
		if (*format == '%') {
			++format;
			if (!*format)
				return;
			switch (*format) {
				case 'd':
				case 'i':
				{
					int n = va_arg(arg, int);
					if (n == INT_MIN) {
						kprint("-2147483648");
						break;
					}
					if (n < 0) {
						kputchar('-');
						n = ~n + 1;
					}
					char lsh = '0' + n % 10;
					n /= 10;
					char buf[9];
					char *p_buf = buf;
					while (n) {
						*p_buf++ = '0' + n % 10;
						n /= 10;
					}
					while (p_buf != buf)
						kputchar(*--p_buf);
					kputchar(lsh);
				}
					break;
				case 'u':
				{
					unsigned n = va_arg(arg, unsigned);
					char lsh = '0' + n % 10;
					n /= 10;
					char buf[9];
					char *p_buf = buf;
					while (n) {
						*p_buf++ = '0' + n % 10;
					n /= 10;
					}
					while (p_buf != buf)
						kputchar(*--p_buf);
					kputchar(lsh);
					}
					break;
				case 'o':
				{
					unsigned n = va_arg(arg, unsigned);
					char lsh = '0' + n % 8;
					n /= 8;
					char buf[10];
					char *p_buf = buf;
					while (n) {
						*p_buf++ = '0' + n % 8;
						n /= 8;
					}
					while (p_buf != buf)
						kputchar(*--p_buf);
					kputchar(lsh);
					}
					break;
				case 'x':
				{
					unsigned n = va_arg(arg, unsigned);
					char lsh = to_hex_digit(n % 16);
					n /= 16;
					char buf[7];
					char *p_buf = buf;
					while (n) {
						*p_buf++ = to_hex_digit(n % 16);
					n /= 16;
					}
					while (p_buf != buf)
					kputchar(*--p_buf);
					kputchar(lsh);
				}
					break;
				case 'X':
				{
					unsigned n = va_arg(arg, unsigned);
					char lsh = to_hex_digit(n % 16);
					n /= 16;
					char buf[7];
					char *p_buf = buf;
					while (n) {
						*p_buf++ = to_hex_digit(n % 16);
						n /= 16;
					}
					while (p_buf != buf)
						kputchar(toupper(*--p_buf));
					kputchar(toupper(lsh));
				}
					break;
				case 'c':
					kputchar(va_arg(arg, int));
					break;
				case 's':
					kprint(va_arg(arg, char *));
					break;
				case 'p':
				{
					kprint("0x");
					size_t ptr = va_arg(arg, size_t);
					char lsh = to_hex_digit(ptr % 16);
					ptr /= 16;
					char buf[15];
					char *p_buf = buf;
					while (ptr) {
						*p_buf++ = to_hex_digit(ptr % 16);
						ptr /= 16;
					}
					while (p_buf != buf)
						kputchar(*--p_buf);
					kputchar(lsh);
				}
					break;
				case '%':
					kputchar('%');
					break;
				default:
					kputchar('%');
				kputchar(*format);
			}
		} else
			kputchar(*format);
		++format;
	}
}

void kprintf(const char *format, ...) {
	va_list arg;
	va_start(arg, format);
	kvprintf(format, arg);
	va_end(arg);
}