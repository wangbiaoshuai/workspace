#include <stdio.h>

int start()
{
    char* argv[] = {"wang", "biao", "shuai"};
    char ***p = (char***)&argv;
    return 0;
}

int main(int argc, char* argv[])
{
    char ***p = &argv;
    return 0;
}
