#pragma once

#define EXPECT_TRUE(condition)  \
    do                          \
    {                           \
        if (!(condition))         \
        {                       \
            while(1);           \
        }                       \
    }                           \
    while(0)                    \

#define EXPECT_FALSE(condition) EXPECT_TRUE(!(condition))

#define EXPECT_EQ(x, y) EXPECT_TRUE((x == y))

#define EXPECT_NEQ(x, y) EXPECT_FALSE((x == y))