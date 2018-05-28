#include "factory.h"
#include <stdio.h>

Factory::Factory()
{
}

Factory::~Factory()
{
}

Product* Factory::CreateProduct(int num)
{
    Product* pro = NULL;
    switch(num)
    {
    case 1:
        pro = new ProductA();
        break;
    case 2:
        pro = new ProductB();
        break;
    default:
        pro = NULL;
        break;
    }
    return pro;
}
