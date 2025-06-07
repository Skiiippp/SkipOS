#include "../inc/string.h"
#include "../inc/printk.h"
#include "../inc/common.h"
#include "../inc/vga.h"
#include "../inc/keyboard.h"
#include "../inc/interrupt.h"
#include "../inc/gdt.h"
#include "../inc/serial.h"
#include "../inc/memory.h"
#include "../inc/assert.h"
#include "../inc/kmalloc.h"

#include <limits.h>

#include "../../unit_test/inc/ut_main.h"
#include "../../system_test/inc/printk_test.h"
#include "../../system_test/inc/pf_alloc_test.h"
#include "../../system_test/inc/kmalloc_test.h"


/**
 * BEGIN PRIVATE
 */

//#define RUN_UNIT_TESTS
//#define RUN_PRINTK_TESTS
//#define RUN_PF_ALLOC_TESTS
//#define RUN_KMALLOC_TESTS

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

    printk("\n\nWELCOME TO SKIPOS\n\n");

    GDT_init();
    printk("GDT inititialized.\n");

    IRQ_init();
    printk("Interrupt system initialized.\n");

    KBD_init();
    printk("Keyboard initialized.\n");

    SER_init();
    printk("Serial output initialized.\n");

    const size_t mem_size = MMU_init_pf(mb_tags_ptr);
    printk("Physical memory system initialized, physical size: 0x%lx.\n", mem_size);

    MMU_init_vp();
    printk("Virtual memory system initialized.\n");

    kmalloc_init();
    printk("Kernel heap allocator initialized.\n");

    IRQ_start();

#ifdef RUN_KMALLOC_TESTS
    kmalloc_test();
#endif

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
