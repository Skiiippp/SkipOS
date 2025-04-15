#include "../inc/string.h"

#include <stddef.h>

#include "../inc/assert.h"
#include "../inc/common.h"

/**
 * BEGIN PRIVATE DEF
 */

static size_t strval(const char *s);

/**
 * END PRIVATE DEF
 */


void *memset(void *s, int c, size_t n)
{
    assert(c <= UINT8_MAX);

    for(size_t i = 0; i < n; i++)
    {
        ((char *)s)[i] = (char)c;
    }

    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    for(size_t i = 0; i < n; i++)
    {
        ((char *)dest)[i] = ((char *)src)[i];
    }

    return dest;
}

size_t strlen(const char *s)
{
    size_t len = 0;

    while(s[len] != '\0')
    {
        len += 1;
    }

    return len;
}

char *strcpy(char *dest, const char *src)
{
    size_t src_len; // Includes null byte

    src_len = strlen(src) + 1;
    memcpy(dest, src, src_len);

    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    return (strval(s1) - strval(s2));
}

char *strchr(const char *s, int c)
{
    int index = 0;
    
    while(s[index] != '\0')
    {
        if(s[index] == c)
        {
            return (char *)&s[index];
        }

        index++;
    }

    if(c == '\0')
    {
        return (char *)&s[index+1];
    }

    return NULL;
}

size_t strval(const char *s)
{
    int index = 0;
    size_t sum = 0;

    assert(s);

    while(s[index] != '\0')
    {
        sum += (unsigned char)s[index];
        index += 1;
    }

    return sum;
}