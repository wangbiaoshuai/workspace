#include <cstdio>
#include <string>
using namespace std;

int main()
{
    try
    {
        printf("this is try.\n");
        throw 10;
    }
    catch(double e)
    {
        printf("catch an int exception. e:%d\n", e);
        return -1;
    }
    return 0;
}

