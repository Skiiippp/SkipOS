#ifndef COMMON_H
#define COMMON_H

#include "../include/types.h"

void panic(const char *, ...);

u64 get_hart_id();
char get_priv_level();

#endif
