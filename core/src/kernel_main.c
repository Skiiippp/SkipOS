#include "../inc/string.h"
#include "../inc/printk.h"
#include "../inc/common.h"
#include "../inc/vga.h"
#include "../inc/keyboard.h"
#include "../inc/interrupt.h"
#include "../inc/gdt.h"
#include "../inc/serial.h"
#include "../inc/memory.h"

#include <limits.h>

#include "../../unit_test/inc/ut_main.h"
#include "../../system_test/inc/printk_test.h"
#include "../../system_test/inc/pf_alloc_test.h"


/**
 * BEGIN PRIVATE
 */

//#define RUN_UNIT_TESTS
//#define RUN_PRINTK_TESTS
#define RUN_PF_ALLOC_TESTS

//#define PAUSE

/**
 * END PRIVATE
 */

int kernel_main(u8 *mb_tags_ptr)
{

#ifdef PAUSE
    int j = 0;
    while(!j);
#endif

    VGA_clear();

    GDT_init();

    IRQ_init();

    KBD_init();

    SER_init();

    MMU_init(mb_tags_ptr);

    IRQ_start();

#ifdef RUN_PF_ALLOC_TESTS
    pf_all_test();
#endif

#ifdef RUN_PRINTK_TESTS
    printk_test();
#endif

#ifdef RUN_SYSTEM_TESTS
    st_main();
#endif

    while(1)
    {
        HLT;
    }

    return -1;
}
