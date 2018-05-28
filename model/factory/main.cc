#include "factory.h"
#include <iostream>
using namespace std;

int main()
{
    Factory fac;
    Product* pro = fac.CreateProduct(2);
    int res = pro->Add(4, 6);
    cout<<"res="<<res<<endl;
    delete pro;
    return 0;
}
