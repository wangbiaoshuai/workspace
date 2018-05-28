#include <stdio.h>
#include <typeinfo>
using namespace std;

class Mammal
{
public:
    ~Mammal() {printf("~Mammal\n");}
    virtual int LayLegs() {return 0;}
    virtual void Myself() {printf("Mammal self\n");};
};

class Cat: public Mammal
{
public:
    ~Cat() {printf("~Cat\n");}
    int LayLegs() {return 1;}
};

class PlatyPus: public Mammal
{
public:
    ~PlatyPus() {printf("~PlatyPus\n");}
    int LayLegs() {return 2;}
};

int main()
{
    Mammal* p;
    p = new Mammal();
    printf("p is pointing to object of %s, LayLegs=%d\n", typeid(*p).name(), p->LayLegs());
    delete p;

    p = new Cat();
    //p->Myself();  //编译错误，因为使用基类指针只能调用基类中已经有的成员函数。
    printf("p is pointing to object of %s, LayLegs=%d\n", typeid(*p).name(), p->LayLegs());
    delete p;
    
    p = new PlatyPus();
    printf("p is pointing to object of %s, LayLegs=%d\n", typeid(*p).name(), p->LayLegs());
    delete p;

    return 0;
}
