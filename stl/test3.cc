#include <map>
#include <stdio.h>
using namespace std;

int main()
{
    map<int, char> my_map;

    for(int i = 0; i < 10; i++)
    {

        my_map.insert(pair<int, char>(i, 'a'+i));
    }

    printf("map[5]=%c---->%p\n", my_map[5], &(my_map[5]));

    printf("erase map[2]=%c---->%p\n", my_map[2], &(my_map[2]));
    my_map.erase(2);

    printf("map[5]=%c---->%p\n", my_map[5], &(my_map[5]));
    return 0;
}
