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

    while(!feof(fp))
    {
        char x = '\0';
        x = fgetc(fp);
        if(feof(fp))
        {
            printf("file test.txt is end.\n");
            break; 
        }

        printf("%c", x);
    }

    fclose(fp);
    return 0;
}
