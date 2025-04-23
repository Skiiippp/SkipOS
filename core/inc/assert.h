#pragma once

#include "common.h"

#define assert(condition)   \
    do                      \
    {                       \
        if (!(condition))    \
        {                   \
            asm volatile ("cli\nhlt");   \
        }                   \
    }                       \
    while(0)                \

