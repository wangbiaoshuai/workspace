#ifndef _PRODUCT_H
#define _PRODUCT_H

#include <iostream>
using namespace std;
class Product
{
public:
    virtual ~Product(){cout<<"Product::~Product"<<endl;}
    virtual int Add(int x, int y) = 0;
protected:
    Product(){cout<<"Product::Product"<<endl;}
};

class ProductA: public Product
{
public:
    ProductA();
    ~ProductA();
    int Add(int x, int y);
};

class ProductB: public Product
{
public:
    ProductB();
    ~ProductB();
    int Add(int ,int);

};
#endif // _PRODUCT_H
