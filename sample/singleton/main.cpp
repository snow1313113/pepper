#include <iostream>
#include "../../include/base_struct.h"
using namespace std;
using namespace Pepper;

class Test : public Singleton<Test>
{
public:
    void foo()
    {
        cout << "cal foo ok" << endl;
    }
};

int main()
{
    Test::instance().foo();
    return 0;
}


