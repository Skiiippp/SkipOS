#pragma once

#include "common.h"
#include "printk.h"
#include "interrupt.h"

#define assert(condition)   \
    do                      \
    {                       \
        if (!(condition))    \
        {                   \
            asm volatile ("cli");   \
            printk("Assert failed: %s:%u\n", __FILE__, __LINE__);   \
            HLT;   \
        }                   \
    }                       \
    while(0)                \


#define assert_noprint(condition)    \
    do  \
    {   \
        if (!(condition))   \
        {   \
            CLI; \
            HLT; \
        }   \
    }   \
    while (0)   \

    
