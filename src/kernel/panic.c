#include "panic.h"
#include "print.h"

void panic(const char *format, ...) {
	kputs("Kernel panic!");
	kputs("Reason:");
	va_list arg;
	va_start(arg, format);
	kvprintf(format, arg);
	va_end(arg);
	asm volatile ("wfi");
}