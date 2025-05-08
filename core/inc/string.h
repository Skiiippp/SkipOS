#pragma once

/**
 * I think the compiler uses built in function protoypes and applies certain attributes, such as pointers being never null, so thats why there isn't error checking for that
 */

#include "common.h"

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

size_t strlen(const char *s);

char *strcpy(char *dest, const char *src);

int strcmp(const char *s1, const char *s2);

char *strchr(const char *s, int c);