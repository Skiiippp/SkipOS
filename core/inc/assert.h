#pragma once

#include "common.h"

#define assert(condition)   \
    do                      \
    {                       \
        if (!(condition))    \
        {                   \
            while(1);       \
        }                   \
    }                       \
    while(0)                \

