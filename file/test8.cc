#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
    FILE* fp = fopen("test.txt", "r");
    if(fp == NULL)
    {
        printf("open file test.txt error: %s\n", strerror(errno));
        return -1;
    }

    int i = 0;
    while(!feof(fp))
    {
        i++;
        char line[512] = {0};
        fgets(line, sizeof(line) - 1, fp);
        /*if(feof(fp))
        {
            printf("test.txt is end.\n");
            break;
        }*/
        /*对line进行处理*/
        printf("line:%s", line);
    }
    printf("i: %d\n", i);
    
    fclose(fp);
    return 0;
}
