#ifndef UART_H
#define UART_H

#include <stddef.h>
#include <stdarg.h>
#include "../../include/types.h"

// 0x10000000 is memory-mapped address of UART according to device tree
#define UART_ADDR 0x10000000

void uart_init(size_t);
void uart_put(size_t, u8);

#endif
