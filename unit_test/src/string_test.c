/**
 * @brief Unit tests for functions defined in string.h
 */

#include "../inc/unit_test.h"

#include <stddef.h>

#include "../../core/inc/string.h"
#include "../../core/inc/common.h"


/**
 * BEGIN PRIVATE DEFS
 */

static void memset_test();
static void memcpy_test();
static void strlen_test();
static void strcpy_test();
static void strcmp_test();
static void strchr_test();

/**
 * END PRIVATE DEFS
 */

void string_test()
{
    memset_test();
    memcpy_test();
    strlen_test();
    strcpy_test();
    strcmp_test();
    strchr_test();
}

void memset_test()
{
    char arr[10];
    u32 sum = 0;


    memset(arr, 13, 10);

    for(int i = 0; i < 10; i++)
    {
        sum += arr[i];
    }

    EXPECT_EQ(sum, 130);

    sum = 0;
    memset(arr, 200, 0);

    for(int i = 0; i < 10; i++)
    {
        sum += arr[i];
    }

    EXPECT_EQ(sum, 130);
}

void memcpy_test()
{
    char arr0[10], arr1[10];

    for(int i = 0; i < 10; i++)
    {
        arr0[i] = i;
        arr1[i] = i + 13;
    }

    memcpy(arr0, arr1, 10);

    for(int i = 0; i < 10; i++)
    {
        EXPECT_EQ(arr0[i], arr1[i]);
    }

    for(int i = 0; i < 10; i++)
    {
        arr0[i] = i;
        arr1[i] = i + 10;
    }

    memcpy(arr0, arr1, 0);

    for(int i = 0; i < 10; i++)
    {
        EXPECT_NEQ(arr0[i], arr1[i]);
    }
}

void strlen_test()
{
    char *s0 = "test", *s1 = "";

    EXPECT_EQ(strlen(s0), 4);
    EXPECT_EQ(strlen(s1), 0);
}

void strcpy_test()
{
    char *s0 = "test", *s1 = "";
    char arr[10];

    for(int i = 0; i < 10; i++)
    {
        arr[i] = 'z';
    }

    strcpy(arr, s0);

    for(int i = 0; i < 5; i++)
    {
        EXPECT_EQ(arr[i], s0[i]);
    }

    strcpy(arr, s1);

    EXPECT_EQ(arr[0], s1[0]);
}

void strcmp_test()
{
    char *s0 = "ABC", *s1 = "CBA", *s2 = "ABZ", *s3 = "AB", *s4 = "";

    EXPECT_TRUE(strcmp(s0, s1) == 0);
    EXPECT_TRUE(strcmp(s0, s2) < 0);
    EXPECT_TRUE(strcmp(s0, s3) > 0);
    EXPECT_TRUE(strcmp(s0, s4) > 0);
}


void strchr_test()
{
    char *s = "test";

    EXPECT_EQ(strchr("test", 't'), &s[0]);
    EXPECT_EQ(strchr(s, '\0'), &s[4]);
    EXPECT_EQ(strchr(s, 'z'), NULL);
}