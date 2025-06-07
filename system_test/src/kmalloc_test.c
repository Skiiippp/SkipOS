#include "../inc/kmalloc_test.h"

#include "../../core/inc/kmalloc.h"
#include "../../core/inc/common.h"
#include "../../core/inc/string.h"
#include "../../core/inc/assert.h"
#include "../../core/inc/memory.h"

void kmalloc_test()
{
    //const u8 pattern = 0xFA;
    //u8 *mem;

    // mem = (u8 *)kmalloc(0xFF5);
    // mem[0xFF4] = pattern;
    // kfree(mem);

    

    // GP fault occur when i == 0xFF2
    // Sometime page fault when i == 0xFF5
    // Looks like some kind of overflow issue - "pattern" shows up in addr faults
    // Seems like things work until you have to allocate more memory than avaliable in the block groups.
    // for(size_t i = 0; i < 10; i++)
    // {
    //     printk("%lx\n", i);
    //     mem = (u8 *)kmalloc(0x1000);
    //     printk("Addr: %p\n", mem);

    //     memset(mem, pattern, 0x1000);
    //     for (size_t j = 0; j < 0x1000; j++)
    //     {
    //         assert(mem[j] == pattern);
    //     }

    //     kfree(mem);
    // }

    // for (size_t i = 1; i < 0x2000; i += 1)
    // {
    //     mem = (u8 *)kmalloc(i);

    //     memset(mem, (u8)i, i);
    //     for(size_t j = 0; j < i; j++)
    //     {
    //         assert(mem[j] == (u8)i);
    //     }

    //     if(i % 0x1000 == 0)
    //     {
    //         printk("On index %lx\n", i);
    //     }

    //     kfree(mem);
    // }

    // for (size_t i = 0x2000; i < 0x2000000; i += 0x10000)
    // {
    //     mem = (u8 *)kmalloc(i);

    //     memset(mem, (u8)i, i);
    //     for(size_t j = 0; j < i; j++)
    //     {
    //         assert(mem[j] == (u8)i);
    //     }

    //     if(i % 0x1000 == 0)
    //     {
    //         printk("On index %lx\n", i);
    //     }

    //     kfree(mem);
    // }


    volatile int *test = (int *)MMU_alloc_page();

    //asm volatile ("invlpg [%0]" :: "r"(test));
    //*test = 42;
    MMU_free_page((void *)test);
    //MMU_free_page((void *)test);
    printk("%d\n", *test);
    (void)test;
    // printk("made it\n");
    // printk("%p, %d\n", test, *test);

    // MMU_free_page((void *)test);

    // printk("%p, %d\n",test, *test);

    // volatile int new = *test;

    // (void)new;
}