#include <stdarg.h>
#include <stdint.h>
#include "common.h"
#include "../uart/uart.h"
#include "../types.h"

int toupper(int c) {
	return 'a' <= c && c <= 'z' ? c + 'A' - 'a' : c;
}

void panic(const char *format, ...) {
	kputs("Kernel panic!");
	kputs("Reason:");
	va_list arg;
	va_start(arg, format);
	kvprintf(format, arg);
	va_end(arg);
	asm volatile ("wfi");
}

uint64 get_hart_id() {
	uint64 x;
	asm volatile("csrr %0, mhartid" : "=r" (x) );
	return x;
}

char get_priv_level() {
	uint64 x;
	asm volatile("csrr %0, mstatus" : "=r" (x) );
	x = (x >> 11) & 0b11;
	switch(x) {
		case 0:
			return 'U';
			break;
		case 1:
			return 'S';
			break;
		case 3:
			return 'M';
			break;

		default: 
			return 'X';
	}
}