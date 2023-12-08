#ifndef PRINT_H
#define PRINT_H

#include <stdarg.h>

int toupper(int c);
int kputchar(int);
int kputs(const char *);
void kvprintf(const char *, va_list);
void kprintf(const char *, ...);

#endif