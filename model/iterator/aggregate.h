#ifndef _AGGREGATE_H
#define _AGGREGATE_H

class Iterator;
typedef int Object;
class Interator;

class Aggregate
{
public:
    virtual ~Aggregate();
    virtual Iterator* CreateIterator() = 0;
    virtual Object GetItem(int) = 0;
    virtual int GetSize() = 0;
protected:
    Aggregate();

};

class ConcreteAggregate : public Aggregate
{
public:
    ConcreteAggregate();
    ~ConcreteAggregate();
    enum{ SIZE = 3;}
    Iterator* CreateIterator();
    Object GetItem(int);
    int GetSize();

private:
    Object obj_[SIZE];
};
#endif // _AGGREGATE_H
