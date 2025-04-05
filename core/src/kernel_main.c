#include "../inc/vga_display.h"
#include "../inc/string.h"

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

    s[0] = 'A';
    s[1] = '\n';

    while(1)
    {
        VGA_display_str(s);
        s[0] += 1;
        for(int i = 0; i < 100000000; i++);
    }

    while(1);
}