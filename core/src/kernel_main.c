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

#define RUN_UNIT_TESTS
#define RUN_SYSTEM_TESTS

/**
 * END PRIVATE
 */

int kernel_main()
{
    VGA_clear();

#ifdef RUN_UNIT_TESTS
    ut_main();
#endif

#ifdef RUN_SYSTEM_TESTS
    st_main();
#endif

    KBD_init();

    KBD_run();

    while(1);
}
