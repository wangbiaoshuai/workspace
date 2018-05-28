#include "aggregate.h"

Aggregate::Aggregate()
{
}

Aggregate::~Aggregate()
{
}

ConcreteAggregate::ConcreteAggregate()
{
    for(int i = 0; i < SIZE; i++)
    {
        obj_[i] = i;
    }
}

ConcreteAggregate::~ConcreteAggregate()
{
}

Iterator* ConcreteAggregate::CreateIterator()
{
    return new ConcreteAggregate(this);
}

Object ConcreteAggregate::GetItem(int i)
{
    if(i < SIZE)
        return obj_[i];
    else
        return -1;
}

int ConcreteAggregate::GetSize()
{
    return SIZE;
}

