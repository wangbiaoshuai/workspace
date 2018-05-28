#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
    FILE * fp = fopen("test.txt", "r");
    if(fp == NULL)
    {
        printf("open file test.txt error: %s\n", strerror(errno));
        return -1;
    }

    rewind(fp);
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);

    char* buf = new char[size + 1];
    memset(buf, 0, size + 1);

    fread(buf, size, 1, fp);
    if(strlen(buf) != 0)
    {
        printf("buf: %s\n", buf);
    }

    delete [] buf;
    fclose(fp);
    return 0;
}
