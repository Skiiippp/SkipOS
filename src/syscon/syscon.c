#include <stdint.h>
#include "syscon.h"
#include "../uart/uart.h"
#include "../types.h"

void poweroff(void) {
	kputs("Poweroff requested");
	*(uint32 *)SYSCON_ADDR = 0x5555;
}

void reboot(void) {
	kputs("Reboot requested");
	*(uint32 *)SYSCON_ADDR = 0x7777;
}
