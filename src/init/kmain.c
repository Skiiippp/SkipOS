#include "../drivers/uart/uart.h"
#include "../kernel/print.h"
#include "../drivers/syscon/syscon.h"
#include "../kernel/common.h"

void kmain(void) {
	uart_init(UART_ADDR);

	kprintf("Hart ID: %x\n", get_hart_id());
	kprintf("Privelage Mode: %c\n", get_priv_level());


	poweroff();
}
