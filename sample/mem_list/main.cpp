#include <iostream>
#include <algorithm>
#include <vector>
#include "../../include/mem_list.h"
using namespace std;
using namespace pepper;

int main()
{
    cout << "-------------test MemList---------" << endl;
    MemList<uint32_t, 5> list;
    cout << "size : " << list.size() << endl;
    list.clear();

    // 测试插入
    list.push_front(3);
    list.push_front(2);
    list.push_front(4);
    list.push_back(6);
    list.push_back(8);
    list.push_back(7);

    // 测试迭代器
    cout << "size : " << list.size() << endl;
    for (auto beg = list.begin(), end = list.end(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;

    // 测试头部和尾部弹出
    list.pop_front();
    list.pop_back();

    cout << "---------------------" << endl;
    cout << "size : " << list.size() << endl;
    for (auto beg = list.begin(), end = list.end(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;

    list.pop_front();
    list.pop_front();
    list.pop_front();

    cout << "---------------------" << endl;
    cout << "size : " << list.size() << endl;
    for (auto beg = list.begin(), end = list.end(); beg != end; ++beg)
    {
        cout << (*beg) << " ";
    }
    cout << endl;

    list.push_front(3);
    list.push_front(2);
    list.push_front(4);
    list.push_front(6);
    list.push_front(8);
    list.push_front(7);

    cout << list.size() << endl;
    for (auto beg : list)
//    for (auto beg = list.begin(), end = list.end(); beg != end; ++beg)
    {
        cout << beg << " ";
    }
    cout << endl;
    return 0;
}

