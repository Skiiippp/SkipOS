#define EXPECT_TRUE(condition)  \
    do                          \
    {                           \
        if (!condition)         \
        {                       \
            while(1);           \
        }                       \
    }                           \
    while(0)                    \

#define EXPECT_EQ(x, y) EXPECT_TRUE(x == y)