#include "product.h"
#include <iostream>
using namespace std;

ProductA::ProductA()
{
    cout<<"ProductA::ProductA"<<endl;
}

ProductA::~ProductA()
{
    cout<<"ProductA::~ProductA"<<endl;
}

int ProductA::Add(int x, int y)
{
    cout<<"ProductA::Add"<<endl;
    return x + y;
}

ProductB::ProductB()
{
    cout<<"ProductB::ProductB"<<endl;
}

ProductB::~ProductB()
{
    cout<<"ProductB::~ProductB"<<endl;
}

int ProductB::Add(int x, int y)
{
    cout<<"ProductB::Add"<<endl;
    return x + y;
}
