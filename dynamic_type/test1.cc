#include <stdio.h>
#include <assert.h>
#include <typeinfo>
using namespace std;

class Mammal
{
public:
    virtual ~Mammal() {};
    int LayLegs() {return 0;}
};

class Cat: public Mammal
{
public:
    int LayLegs() {return 1;}
};

class PlatyPus: public Mammal
{
public:
    int LayLegs() {return 2;}
};

int main()
{
    Mammal* p;
    Mammal any_mammal;
    Cat cat;
    PlatyPus platypus;

    p = &any_mammal;
    printf("p is pointing to an object of %s, LayLegs=%d\n", typeid(*p).name(), p->LayLegs());

    p = &cat;
    assert(typeid(*p) == typeid(Cat));
    printf("p is pointing to an object of %s, LayLegs=%d\n", typeid(*p).name(), p->LayLegs());

    p = &platypus;
    printf("p is pointing to an object of %s, LayLegs=%d\n", typeid(*p).name(), p->LayLegs());

    return 0;
}
