#include <vector>
#include <iostream>
#include "../../include/fixed_mem_pool.h"
using namespace std;
using namespace Pepper;

struct Test
{
    uint32_t a;
    size_t b;
    uint32_t c;
};

int main()
{
    FixedMemPool<Test> pool;
    static const size_t MAX_NUM = 100;
    uint64_t temp[MAX_NUM] = {0};

    // 产生一个数组
    for (size_t i = 0; i < MAX_NUM; ++i)
        temp[i] = i * i;
 
    static const size_t node_size = sizeof(Test) + 10;
    static const size_t mem_size = FixedMemPool<Test>::CalcNeedSize(MAX_NUM, node_size);
    uint8_t * mem = new uint8_t[mem_size];
    if (pool.Init(mem, mem_size, node_size, true) == false)
    {
        cout << "init error" << endl;
        return 0;
    }

    for (size_t i = 0; i < MAX_NUM; ++i)
    {
        Test * p = pool.Alloc();
        if (p == NULL)
        {
            cout << "alloc null" << endl;
            return 0;
        }
        p->a = i + 1;
        p->b = temp[i];
        p->c = i * i;
    }

    if (pool.Size() != MAX_NUM)
    {
        cout << "size error " << pool.Size() << endl;
        return 0;
    }

    if (pool.Size() != pool.Capacity())
    {
        cout << "size " << pool.Size() << " != capacity " << pool.Capacity() << endl;
        return 0;
    }

    if (pool.Alloc() != NULL)
    {
        cout << "is full but alloc succe" << endl;
        return 0;
    }

    cout << "************ after alloc *************" << endl;
    cout << "size " << pool.Size() << " capacity " << pool.Capacity() << endl;
    vector<const Test*> vec;
    size_t count = 0;
    for (auto beg = pool.Begin(), end = pool.End(); beg != end; ++beg)
    {
        size_t index = pool.Ptr2Int(&(*beg));
        auto p = pool.Int2Ptr(index);
        cout << "(a = " << p->a << " b = " << p->b << " c = " << p->c << "), ";
        ++count;
        if (count % 2 == 0)
            vec.push_back(p);
        if (count % 5 == 0)
            cout << endl;
    }

    for (auto beg = vec.begin(), end = vec.end(); beg != end; ++beg)
    {
        bool result = pool.Free(*beg);
        if (!result)
        {
            cout << "free error" << endl;
            return 0;
        }
    }

    // 重复删除，应该都不能成功才对
    for (auto beg = vec.begin(), end = vec.end(); beg != end; ++beg)
    {
        bool result = pool.Free(*beg);
        if (result)
        {
            cout << "free again succe, it war error" << endl;
            return 0;
        }
    }

    cout << "************ after free 1/2 *************" << endl;
    cout << "size " << pool.Size() << " capacity " << pool.Capacity() << endl;
    count = 0;
    for (auto beg = pool.Begin(), end = pool.End(); beg != end; ++beg)
    {
        size_t index = pool.Ptr2Int(&(*beg));
        auto p = pool.Int2Ptr(index);
        cout << "(a = " << p->a << " b = " << p->b << " c = " << p->c << "), ";
        ++count;
        if (count % 5 == 0)
            cout << endl;
     }
    
    pool.Clear();
    cout << "************ after clear *************" << endl;
    cout << "size " << pool.Size() << " capacity " << pool.Capacity() << endl;
    count = 0;
    for (auto beg = pool.Begin(), end = pool.End(); beg != end; ++beg)
    {
        size_t index = pool.Ptr2Int(&(*beg));
        auto p = pool.Int2Ptr(index);
        cout << "(a = " << p->a << " b = " << p->b << " c = " << p->c << "), ";
        ++count;
        if (count % 5 == 0)
            cout << endl;
    }

    return 0;
}


