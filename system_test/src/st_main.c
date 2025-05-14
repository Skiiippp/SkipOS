#include "../inc/st_main.h"

#include "../inc/printk_test.h"
#include "../inc/pf_alloc_test.h"

int st_main()
{
    printk_test();
    pf_all_test();

    return 0;
}