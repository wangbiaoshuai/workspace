#ifndef _FACTORY_H
#define _FACTORY_H

#include "product.h"

class FactoryItf
{
public:
    virtual ~FactoryItf(){}
    virtual Product* CreateProduct(int) = 0;

protected:
    FactoryItf(){}
};

class Factory: public FactoryItf
{
public:
    Factory();
    ~Factory();
    Product* CreateProduct(int);
};

#endif // _FACTORY_H
