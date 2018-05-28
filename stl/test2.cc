#include <vector>
#include <stdio.h>
using namespace std;

int main()
{
    vector<int> vec;
    for(int i = 0; i < 10; i++)
    {
        vec.push_back(i);
    }

    printf("vec[5]=%d---->%p\n", vec[5], &(vec[5]));

    vector<int>::iterator it = vec.begin() + 1;
    printf("earse vector[1]=%d---->%p\n", *it, &(*it));
    vec.erase(it);

    printf("vec[5]=%d---->%p\n", vec[5], &(vec[5]));
    
    return 0;
}
