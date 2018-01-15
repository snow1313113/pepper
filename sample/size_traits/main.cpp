#include <iostream>
#include "../../include/utils/traits_utils.h"
using namespace std;
using namespace Pepper;

int main()
{
    FixSize<0x0>::IntType a0;
    FixSize<0x1>::IntType a4;
    FixSize<0x11>::IntType a8;
    FixSize<0x111>::IntType a12;
    FixSize<0x1111>::IntType a16;
    FixSize<0x11111>::IntType a20;
    FixSize<0x111111>::IntType a24;
    FixSize<0x1111111>::IntType a28;
    FixSize<0x11111111>::IntType a32;
    FixSize<0x111111111>::IntType a36;
    FixSize<0x1111111111>::IntType a40;
    cout << "a0  :" << sizeof(a0) << endl;
    cout << "a4  :" << sizeof(a4) << endl;
    cout << "a8  :" << sizeof(a8) << endl;
    cout << "a12 :" << sizeof(a12) << endl;
    cout << "a16 :" << sizeof(a16) << endl;
    cout << "a20 :" << sizeof(a20) << endl;
    cout << "a24 :" << sizeof(a24) << endl;
    cout << "a28 :" << sizeof(a28) << endl;
    cout << "a32 :" << sizeof(a32) << endl;
    cout << "a36 :" << sizeof(a36) << endl;
    cout << "a40 :" << sizeof(a40) << endl;
    return 0;
}


