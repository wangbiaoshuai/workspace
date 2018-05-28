#include <stdio.h>
#include <string.h>
#include <errno.h>

int main()
{
    FILE* fp = fopen("aaa", "a");
    if(fp == NULL)
    {
        printf("open file error: %s\n", strerror(errno));
        return -1;
    }
    printf("open file success.\n");
    fclose(fp);

    fp = fopen("bbb", "r+");
    if(NULL == fp)
    {
        printf("open file bbb error: %s\n", strerror(errno));
        return -1;
    }
    printf("open file bbb success\n");
    fclose(fp);
    return 0;
}
