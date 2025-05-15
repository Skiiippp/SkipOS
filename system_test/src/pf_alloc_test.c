#include "../inc/pf_alloc_test.h"

#include "../../core/inc/memory.h"
#include "../../core/inc/assert.h"
#include "../../core/inc/printk.h"
#include "../../core/inc/string.h"

#include <stddef.h>

#define NUM_ADDRS 5

void pf_simple_test()
{
    void *addrs[NUM_ADDRS];
    void *cpy_addrs[NUM_ADDRS];

    for (int i = 0; i < NUM_ADDRS; i++)
    {
        addrs[i] = MMU_pf_alloc();
        printk("Alloced page at %p.\n", addrs[i]);
        cpy_addrs[i] = addrs[i];
    }

    for (int i = 0; i < NUM_ADDRS; i++)
    {
        for (int j = i; j >= 0; j--)
        {
            MMU_pf_free(addrs[j]);
            printk("\tFreed page at %p.\n", addrs[j]);
        }

        for (int j = 0; j <= i; j++)
        {
            addrs[j] = MMU_pf_alloc();
            printk("\tAlloced page at %p, should be %p.\n", addrs[j], cpy_addrs[j]);
            assert(addrs[j] == cpy_addrs[j]);
        }
    }

    for (int i = 0; i < NUM_ADDRS; i++)
    {
        MMU_pf_free(addrs[i]);
    }
}

void pf_stress_test()
{
    void *new_page;
    size_t page_cnt = 0;
    u64 filler[PAGE_SIZE/sizeof(u64)];

    // int j = 0;
    // while(!j);

    while((new_page = MMU_pf_alloc()) != NULL)
    {
        for (u64 i = 0; i < PAGE_SIZE/sizeof(u64); i++)
        {
            filler[i] = (u64)new_page;
        }

        memcpy(new_page, filler, PAGE_SIZE);
        
        for (int i = 0; i < PAGE_SIZE; i++)
        {
            assert(((u8 *)new_page)[i] == ((u8 *)filler)[i]);
        }

        if (page_cnt % 1000 == 0)
        {
            printk("On page %lu at addr %p\n", page_cnt, new_page);
        }
        
        page_cnt++;
    }

    printk("Total pages alloced: %lu\n", page_cnt);
}

void pf_all_test()
{
    pf_simple_test();
    pf_stress_test();    
}