#include <iostream>
#include "../../include/utils/traits_utils.h"
using namespace std;
using namespace Pepper;

int main()
{
    cout << CalcPrime<1>::PRIME << endl;
    cout << CalcPrime<2>::PRIME << endl;
    cout << CalcPrime<3>::PRIME << endl;
    cout << CalcPrime<4>::PRIME << endl;
    cout << CalcPrime<5>::PRIME << endl;
    cout << CalcPrime<10>::PRIME << endl;
    cout << CalcPrime<50>::PRIME << endl;
    cout << CalcPrime<100>::PRIME << endl;
    cout << CalcPrime<200>::PRIME << endl;
    cout << CalcPrime<1000>::PRIME << endl;
    cout << CalcPrime<1500>::PRIME << endl;
    return 0;
}


