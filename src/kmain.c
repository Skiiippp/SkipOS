#include "uart/uart.h"
#include "syscon/syscon.h"
#include "common/common.h"

void kmain(void) {
	uart_init(UART_ADDR);

	kprintf("Hart ID: %x\n", get_hart_id());
	kprintf("Privelage Mode: %c\n", get_priv_level());


	poweroff();
}
