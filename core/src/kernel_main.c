#include "../../unit_test/inc/unit_main.h"

#define RUN_UNIT_TESTS

int kernel_main()
{
    
#ifdef RUN_UNIT_TESTS
    unit_main();
#endif

}