#include <iostream>
#include "fixed_mem_pool_test.h"
#include "ring_buffer_test.h"

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
