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

void printk_test()
{
    printk("%c\n", 'a'); // should be "a"
    printk("%c\n", 'Q'); // should be "Q"
    printk("%c\n", 256 + '9'); // Should be "9"
    printk("%s\n", "test string"); // "test string"
    printk("foo%sbar\n", "blah"); // "fooblahbar"
    printk("foo%%sbar\n"); // "foo%bar"
    printk("%d\n", INT_MIN); // "-2147483648"
    printk("%d\n", INT_MAX); // "2147483647"
    printk("%u\n", 0); // "0"
    printk("%u\n", UINT_MAX); // "4294967295"
    printk("%x\n", 0xDEADbeef); // "deadbeef"
    printk("%p\n", (void*)UINTPTR_MAX); // "0xFFFFFFFFFFFFFFFF"
    printk("%hd\n", 0x8000); // "-32768"
    printk("%hd\n", 0x7FFF); // "32767"
    printk("%hu\n", 0xFFFF); // "65535"
    printk("%ld\n", LONG_MIN); // "-9223372036854775808"
    printk("%ld\n", LONG_MAX); // "9223372036854775807"
    printk("%lu\n", ULONG_MAX); // "18446744073709551615"
    printk("%qd\n", LONG_MIN); // "-9223372036854775808"
    printk("%qd\n", LONG_MAX); // "9223372036854775807"
    printk("%qu\n", ULONG_MAX); // "18446744073709551615"
}