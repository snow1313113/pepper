#include <iostream>
#include "../../include/structure/fix_mem_pool.h"
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
    if (mem_pool.init() == false)
    {
        cout << "init error" << endl;
    }
    
    return 0;
}


