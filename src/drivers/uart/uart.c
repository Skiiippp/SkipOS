#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include "uart.h"
#include "../../kernel/common.h"
#include "../../include/types.h"

/*
 * Initialize NS16550A UART
 */
void uart_init(size_t base_addr) {
	volatile u8 *ptr = (u8 *)base_addr;

	// Set word length to 8 (LCR[1:0])
	const u8 LCR = 0b11;
	ptr[3] = LCR;

	// Enable FIFO (FCR[0])
	ptr[2] = 0b1;

	// Enable receiver buffer interrupts (IER[0])
	ptr[1] = 0b1;

	// For a real UART, we need to compute and set the baud rate
	// But since this is an emulated UART, we don't need to do anything
	//
	// Assuming clock rate of 22.729 MHz, set signaling rate to 2400 baud
	// divisor = ceil(CLOCK_HZ / (16 * BAUD_RATE))
	//				 = ceil(22729000 / (16 * 2400))
	//				 = 592
	//
	// uint16 divisor = 592;
	// uint8 divisor_least = divisor & 0xFF;
	// uint8 divisor_most = divisor >> 8;
	// ptr[3] = LCR | 0x80;
	// ptr[0] = divisor_least;
	// ptr[1] = divisor_most;
	// ptr[3] = LCR;
}

void uart_put(size_t base_addr, u8 c) {
	*(u8 *)base_addr = c;
}

