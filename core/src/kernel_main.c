#include "../inc/string.h"
#include "../inc/printk.h"
#include "../inc/common.h"
#include "../inc/vga.h"
#include "../inc/keyboard.h"
#include "../inc/interrupt.h"

#include <limits.h>

#include "../../unit_test/inc/ut_main.h"
#include "../../system_test/inc/st_main.h"

/**
 * BEGIN PRIVATE
 */

//#define RUN_UNIT_TESTS
//#define RUN_SYSTEM_TESTS
//define DBG

/**
 * END PRIVATE
 */

int kernel_main()
{
#ifdef RUN_UNIT_TESTS
    ut_main();
#endif

#ifdef RUN_SYSTEM_TESTS
    st_main();
#endif

#ifdef DBG
    int j = 0;
    while(!j);
#endif

    VGA_clear();

    KBD_init();

    IRQ_init();

    // Trigger page fault
    volatile int* ptr = (int*)0xFFFFFFFF00000000;
    int val = *ptr;
    (void)val;

    KBD_run();

    while(1);
}
