#include <iostream>
#include <algorithm>
#include <vector>
#include "../../include/mem_set.h"
using namespace std;
using namespace Pepper;

int main()
{
    static const size_t SIZE = 100;
    typedef MemSet<uint64_t, SIZE> MSet;
    MSet s;
    s.Clear();

    vector<uint64_t> v;
    for (size_t i = 0; i < SIZE; ++i)
        v.push_back(i + 1);
    random_shuffle(v.begin(), v.end());

    cout << "use " << s.Capacity() * sizeof(MSet::ValueType) * 100.0 / sizeof(MSet) << "%" << endl;

    for (auto beg = v.begin(), end = v.end();
            beg != end; ++beg)
    {
        auto it = s.Insert(*beg);
        if (it == s.End())
            cout << "insert " << (*beg) << " error" << endl;
        else
            cout << "insert " << (*beg) << endl;
    }

    cout << "MemSet size " << s.Size() << endl;

    return 0;
}


