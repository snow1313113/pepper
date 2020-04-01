#include <iostream>
#include "../../include/utils/traits_utils.h"
using namespace std;
using namespace pepper;

int main()
{
    cout << NearByPrime<1>::PRIME << endl;
    cout << NearByPrime<2>::PRIME << endl;
    cout << NearByPrime<3>::PRIME << endl;
    cout << NearByPrime<4>::PRIME << endl;
    cout << NearByPrime<5>::PRIME << endl;
    cout << NearByPrime<10>::PRIME << endl;
    cout << NearByPrime<50>::PRIME << endl;
    cout << NearByPrime<100>::PRIME << endl;
    cout << NearByPrime<200>::PRIME << endl;
    cout << NearByPrime<1000>::PRIME << endl;
    cout << NearByPrime<1500>::PRIME << endl;
    cout << NearByPrime<2000>::PRIME << endl;
    cout << NearByPrime<10000>::PRIME << endl;
    cout << NearByPrime<100000>::PRIME << endl;
    cout << NearByPrime<799999>::PRIME << endl;
    cout << NearByPrime<800000>::PRIME << endl;
    cout << NearByPrime<800001>::PRIME << endl;
    cout << NearByPrime<1000000>::PRIME << endl;
    return 0;
}


