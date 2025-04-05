#include "../inc/string.h"

#include "../inc/assert.h"

/**
 * BEGIN PRIVATE DEF
 */

static size_t strval(const char *s);

/**
 * END PRIVATE DEF
 */


void *memset(void *s, int c, size_t n)
{
    assert(s);

    for(size_t i = 0; i < n; i++)
    {
        ((char *)s)[i] = (char)c;
    }

    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    assert(dest && src);

    for(size_t i = 0; i < n; i++)
    {
        ((char *)dest)[i] = ((char *)src)[i];
    }

    return dest;
}

size_t strlen(const char *s)
{
    size_t len = 0;

    assert(s);

    while(s[len] != '\0')
    {
        len += 1;
    }

    return len;
}

char *strcpy(char *dest, const char *src)
{
    size_t src_len; // Includes null byte

    assert(dest && src);

    src_len = strlen(src) + 1;
    memcpy(dest, src, src_len);

    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    assert(s1 && s2);

    return (strval(s1) - strval(s2));
}

const char *strchr(const char *s, int c)
{
    
}

char *strdup(const char *s)
{

}

size_t strval(const char *s)
{
    int index = 0;
    size_t sum = 0;

    assert(s);

    while(s[index] != '\0')
    {
        sum += (unsigned char)s[index];
    }

    return sum;
}