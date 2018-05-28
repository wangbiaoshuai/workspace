#include <stdio.h>
#include <string.h>

int main()
{
    char my_str[] = "wang biao shuai";

    char* res = strtok(my_str, " ");
    while(res)
    {
        printf("str: %s\n", res);
        res = strtok(NULL, " ");
    }

    return 0;
}
