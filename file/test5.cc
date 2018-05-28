#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
    FILE* fp = fopen("file_in.txt", "r");
    if(NULL == fp)
    {
        printf("open file file_in.txt error: %s\n", strerror(errno));
        return -1;
    }

    char buf[255] = {0};
    fgets(buf, sizeof(buf), fp);
    fclose(fp);

    printf("buf: %s\n", buf);
    return 0;
}
