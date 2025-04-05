#include "../inc/vga_display.h"
#include "../inc/string.h"

#include "../../unit_test/inc/unit_main.h"


#define RUN_UNIT_TESTS

/**
 * BEGIN PRIVATE
 */

static inline void halt();

/**
 * END PRIVATE
 */

int kernel_main()
{
    int j = 0;
    
#ifdef RUN_UNIT_TESTS
    unit_main();
#endif

    while(!j);

    VGA_clear();

    while(1);
}

void halt()
{
    __asm__ __volatile__ ("hlt");
}