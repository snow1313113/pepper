#include <iostream>
#include <algorithm>
#include <vector>
#include "../../include/mem_map.h"
using namespace std;
using namespace Pepper;

int main()
{
    cout << "-------------test MemMap---------" << endl;
    static const size_t MAX_SIZE = 100;
    typedef MemMap<uint64_t, int32_t, MAX_SIZE> MyMap;
    size_t max_num = 1000000000;
    MyMap s;
    cout << "MemMap size " << sizeof(s)
        << " need ValueSize " << sizeof(MyMap::T) * s.Capacity()
        << " use " << sizeof(MyMap::T) * s.Capacity() * 100 / static_cast<double>(sizeof(s)) << "%"
        << endl;
    s.Clear();

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

    static const int32_t VALUE = -100;

    // 测试插入
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        s.Insert(temp[i], VALUE - i);
    }
    cout << "after insert : size " << s.Size() << endl;

    // 测试是否满了能否插入
    auto result_it = s.Insert(max_num + 1, VALUE);
    if (result_it.second)
    {
        cout << "insert full succe, key : " << max_num + 1
            << " value : " << VALUE
            << " sizeof " << s.Size() << endl;
    }

    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        auto it = s.Find(temp[i]);
        if (s.End() == it)
            cout << "find err " << temp[i] << endl;
    }

    // 测试迭代器访问
    size_t count = 0;
    for (auto beg = s.Begin(), end = s.End();
            beg != end; ++beg)
    {
        bool is_exist = false;
        for (size_t i = 0; i < MAX_SIZE; ++i)
        {
            if ((*beg).first == temp[i])
            {
                is_exist = true;
                if ((*beg).second != static_cast<int32_t>(VALUE - i))
                    cout << "ERROR: value no equal" << endl;
                break;
            }
        }
        if (!is_exist)
            cout << "ERROR: key(" << (*beg).first << ") no exist" << endl;

        cout << (*beg).first << "(" << (*beg).second << ") ";
        ++count;
        if (count % 5 == 0)
            cout << endl;
    }

    // 测试删除
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        s.Erase(temp[i]);
    }
    cout << "after erase : size " << s.Size() << endl;

    auto it = s.Begin();
    if (it == s.End())
        cout << "beg == end when set is empty " << endl;

    // 测试是否能插入重复的数据
    for (size_t i = 0; i < MAX_SIZE / 2; ++i)
    {
        auto result_it = s.Insert(temp[i], VALUE - i);
        if (!result_it.second)
        {
            cout << "insert, key : " << temp[i]
                << " value : " << VALUE - i
                << " error, size " << s.Size() << endl;
        }
    }
    for (size_t i = 0; i < MAX_SIZE / 2; ++i)
    {
        auto result_it = s.Insert(temp[i], VALUE - i);
        if (result_it.second)
        {
            cout << "insert the same, key :  " << temp[i]
                << " value : " << VALUE - i
                << " sizeof " << s.Size() << endl;
        }
    }
    cout << "after insert: size " << s.Size() << endl;

    // 测试清除
    s.Clear();
    cout << "after clear: size " << s.Size() << endl;
    return 0;
}


