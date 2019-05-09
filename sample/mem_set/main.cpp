#include <iostream>
#include <algorithm>
#include <vector>
#include "../../include/mem_set.h"
using namespace std;
using namespace Pepper;

int main()
{
    cout << "-------------test MemSet---------" << endl;
    static const size_t MAX_SIZE = 100;
    typedef MemSet<uint64_t, MAX_SIZE> MySet;
    size_t max_num = 1000000000;
    MySet s;
    cout << "MemSet size " << sizeof(s)
        << " need ValueSize " << sizeof(MySet::ValueType) * s.capacity()
        << " use " << sizeof(MySet::ValueType) * s.capacity() * 100 / static_cast<double>(sizeof(s)) << "%"
        << endl;
    s.clear();

    // 产生一个随机数组
    uint64_t temp[MAX_SIZE] = {0};
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        if (i == 50)
            temp[i] = 0;
        else
            //temp[i] = arand_range(1, max_num);
            temp[i] = i * (i + 1) + 1;
    }

    // 测试插入
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        s.insert(temp[i]);
    }
    cout << "after insert : size " << s.size() << endl;

    // 测试是否满了能否插入
    auto result_it = s.insert(max_num + 1);
    if (result_it.second)
    {
        uint64_t temp = *(result_it.first);
        cout << "insert full succe " << temp
            << " sizeof " << s.size() << endl;
    }

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto it = s.find(temp[i]);
        if (s.end() == it)
            cout << "find err " << temp[i] << endl;
    }

    // 测试迭代器访问
    size_t count = 0;
    for (auto beg = s.begin(), end = s.end();
            beg != end; ++beg)
    {
        cout << (*beg) << " ";
        ++count;
        if (count % 5 == 0)
            cout << endl;
    }

    // 测试删除
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        s.erase(temp[i]);
    }
    cout << "after erase : size " << s.size() << endl;

    auto it = s.begin();
    if (it == s.end())
        cout << "beg == end when set is empty " << endl;

    // 测试是否能插入重复的数据
    for (size_t i = 0; i < MAX_SIZE / 2; ++i)
    {
        auto result_it = s.insert(temp[i]);
        if (!result_it.second)
        {
            uint64_t temp = *(result_it.first);
            cout << "insert " << temp
                << " error, size " << s.size() << endl;
        }
    }
    for (size_t i = 0; i < MAX_SIZE / 2; ++i)
    {
        auto result_it = s.insert(temp[i]);
        if (result_it.second)
        {
            uint64_t temp = *(result_it.first);
            cout << "insert the same " << temp
                << " sizeof " << s.size() << endl;
        }
    }
    cout << "after insert: size " << s.size() << endl;

    // 测试清除
    s.clear();
    cout << "after clear: size " << s.size() << endl;

    //    static const size_t MAX_SIZE_2 = 50;
    //    typedef MemSet<uint64_t, MAX_SIZE_2> MySet2;
    //    size_t max_num_2 = 1000000000;
    return 0;
}


