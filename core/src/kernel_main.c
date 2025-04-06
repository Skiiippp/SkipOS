#include "../inc/vga_display.h"
#include "../inc/string.h"
#include "../inc/printk.h"

#include "../../unit_test/inc/unit_main.h"

#define RUN_UNIT_TESTS

/**
 * BEGIN PRIVATE
 */



/**
 * END PRIVATE
 */

int kernel_main()
{
    int j = 0;
    char s[2];    
    
#ifdef RUN_UNIT_TESTS
    unit_main();
#endif

    //while(!j);

    VGA_clear();

    test();

    while(1);
}