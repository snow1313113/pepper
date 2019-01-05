#include <iostream>
#include <algorithm>
#include <vector>
#include "../../include/mem_list.h"
using namespace std;
using namespace Pepper;

int main()
{
    cout << "-------------test MemList---------" << endl;
    MemList<uint32_t, 5> list;
    cout << "size : " << list.Size() << endl;
    list.Clear();

    // 测试插入
    list.PushFront(3);
    list.PushFront(2);
    list.PushFront(4);
    list.PushBack(6);
    list.PushBack(8);
    list.PushBack(7);

    // 测试迭代器
    cout << "size : " << list.Size() << endl;
    for (auto beg = list.Begin(), end = list.End(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;

    // 测试头部和尾部弹出
    list.PopFront();
    list.PopBack();

    cout << "---------------------" << endl;
    cout << "size : " << list.Size() << endl;
    for (auto beg = list.Begin(), end = list.End(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;

    list.PopFront();
    list.PopFront();
    list.PopFront();

    cout << "---------------------" << endl;
    cout << "size : " << list.Size() << endl;
    for (auto beg = list.Begin(), end = list.End(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;

    list.PushFront(3);
    list.PushFront(2);
    list.PushFront(4);
    list.PushFront(6);
    list.PushFront(8);
    list.PushFront(7);

    cout << list.Size() << endl;
    for (auto beg = list.Begin(), end = list.End(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;
    return 0;
}

