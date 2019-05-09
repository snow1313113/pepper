#include <iostream>
#include "../../include/ring_buffer.h"
using namespace std;
using namespace Pepper;

struct Test
{
    char a;
    size_t b;
};

int main()
{
    FixedRingBuffer<Test, 5> queue;
//    static_assert(std::is_trivial<FixedRingBuffer<Test, 5> >::value, "must be trivial");
    static_assert(std::is_trivially_copyable<FixedRingBuffer<Test, 5> >::value, "must be trivial");
//    queue.clear();
    cout << "queue capacity: " << queue.capacity() << " , size: " << queue.size() << endl;

    Test test;
    test.a = 'a';
    test.b = 1;
    queue.push(test);
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    test.a = 'a';
    test.b = 2;
    queue.push(test);
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    test.a = 'a';
    test.b = 3;
    queue.push(test);
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    test.a = 'a';
    test.b = 4;
    queue.push(test);
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    test.a = 'a';
    test.b = 5;
    queue.push(test);
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;

    if (queue.full())
        cout << "queue is full, size: " << queue.size() << endl;
    else
        cout << "queue is not full, size: " << queue.size() << endl;

    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    queue.pop();
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    queue.pop();
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    queue.pop();
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    queue.pop();
    cout << "queue begin: " << queue.front().b << " end: " << queue.back().b << endl;
    queue.pop();

    if (queue.empty())
        cout << "queue is empty, size: " << queue.size() << endl;
    else
        cout << "queue is not empty, size: " << queue.size() << endl;
    return 0;
}


