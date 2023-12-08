#include <stdarg.h>
#include <stdint.h>
#include "common.h"
#include "../drivers/uart/uart.h"
#include "../include/types.h"

u64 get_hart_id() {
	u64 x;
	asm volatile("csrr %0, mhartid" : "=r" (x) );
	return x;
}

char get_priv_level() {
	u64 x;
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