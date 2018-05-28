#include <vector>
#include <cstdio>
using namespace std;

int main()
{
    vector<int> vec;
    printf("begin, vec size=%d, capacity=%d\n", vec.size(), vec.capacity());
    for(int i = 0; i < 10; i++)
    {
        vec.push_back(i);
    }

    printf("after push, size=%d, capacity=%d\n", vec.size(), vec.capacity());

    //vec.clear();
    //printf("after clear, size=%d, capacity=%d\n", vec.size(), vec.capacity());

    //vector<int> tmp(vec);
    //vec.swap(tmp);'
    vector<int>().swap(vec);
    printf("after swap, vec size=%d, capacity=%d\n", vec.size(), vec.capacity());
    //printf("after swap, tmp size=%d, capacity=%d\n", tmp.size(), tmp.capacity());

    return 0;
}
