#ifndef TYPES_H
#define TYPES_H

#define to_hex_digit(n) ('0' + (n) + ((n) < 10 ? 0 : 'a' - '0' - 10))

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long s64;

#endif