#include <iostream>
#include "../../include/fixed_mem_pool.h"
using namespace std;
using namespace Pepper;

struct Test
{
    char a;
    size_t b;
};

int main()
{
    FixedMemPool<Test> mem_pool;
    uint8_t * mem = new uint8_t[sizeof(Test) * 1000];
    if (mem_pool.init(mem, sizeof(Test) * 1000) == false)
    {
        cout << "init error" << endl;
    }
    
    return 0;
}


