#include "../inc/string.h"
#include "../inc/printk.h"
#include "../inc/stddef.h"
#include "../inc/vga.h"
#include "../inc/keyboard.h"

#include <limits.h>

#include "../../unit_test/inc/unit_main.h"

#define RUN_UNIT_TESTS

/**
 * BEGIN PRIVATE
 */

static void printk_test();

/**
 * END PRIVATE
 */

int kernel_main()
{
    int j = 0;
    
#ifdef RUN_UNIT_TESTS
    unit_main();
#endif

    //while(!j);

    VGA_clear();
    
    KBD_init();

    KBD_run();

    while(1);
}