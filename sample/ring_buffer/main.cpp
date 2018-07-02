#include <iostream>
#include "../../include/structure/ring_buffer.h"
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
    Test test;
    queue.Clear();

    cout << "queue capacity: " << queue.Capacity() << " , size: " << queue.Size() << endl;

    test.a = 'a';
    test.b = 1;
    queue.Push(test);
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    test.a = 'a';
    test.b = 2;
    queue.Push(test);
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    test.a = 'a';
    test.b = 3;
    queue.Push(test);
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    test.a = 'a';
    test.b = 4;
    queue.Push(test);
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    test.a = 'a';
    test.b = 5;
    queue.Push(test);
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;

    if (queue.IsFull())
        cout << "queue is full, size: " << queue.Size() << endl;
    else
        cout << "queue is not full, size: " << queue.Size() << endl;

    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    queue.Pop();
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    queue.Pop();
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    queue.Pop();
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    queue.Pop();
    cout << "queue begin: " << queue.Front().b << " end: " << queue.Back().b << endl;
    queue.Pop();

    if (queue.IsEmpty())
        cout << "queue is empty, size: " << queue.Size() << endl;
    else
        cout << "queue is not empty, size: " << queue.Size() << endl;
    return 0;
}


