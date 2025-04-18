#pragma once

/**
 * I think the compiler uses built in function protoypes and applies certain attributes, such as pointers being never null, so thats why there isn't error checking for that
 */

#include "common.h"

extern void *memset(void *s, int c, size_t n);

extern void *memcpy(void *dest, const void *src, size_t n);

extern size_t strlen(const char *s);

extern char *strcpy(char *dest, const char *src);

extern int strcmp(const char *s1, const char *s2);

extern char *strchr(const char *s, int c);