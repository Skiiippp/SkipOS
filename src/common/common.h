#ifndef COMMON_H
#define COMMON_H

#include "../types.h"

int toupper(int);
void panic(const char *, ...);

uint64 get_hart_id();
char get_priv_level();

#endif
