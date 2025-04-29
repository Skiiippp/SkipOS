#pragma once

#include "common.h"
#include "printk.h"

#define assert(condition)   \
    do                      \
    {                       \
        if (!(condition))    \
        {                   \
            asm volatile ("cli");   \
            printk("Assert failed: %s:%u\n", __FILE__, __LINE__);   \
            asm volatile ("hlt");   \
        }                   \
    }                       \
    while(0)                \


#define assert_noprint(condition)    \
    do  \
    {   \
        if (!(condition))   \
        {   \
            asm volatile("cli;hlt"); \
        }   \
    }   \
    while (0)   \

    
