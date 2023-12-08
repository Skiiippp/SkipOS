#include "syscon.h"
#include "../../kernel/print.h"
#include "../../include/types.h"


void poweroff(void) {
	kputs("Poweroff requested");
	*(u32 *)SYSCON_ADDR = 0x5555;
}

void reboot(void) {
	kputs("Reboot requested");
	*(u32 *)SYSCON_ADDR = 0x7777;
}
