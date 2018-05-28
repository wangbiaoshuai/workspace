#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <stdlib.h>

using namespace std;

int main()
{
    string cmd = "/root/workspace/process/test3.sh";
    FILE* fp = popen(cmd.c_str(), "w");
    if(fp == NULL)
    {
        printf("popen: %s\n", strerror(errno));
        return -1;
    }

    int status = pclose(fp);
    printf("pclose: cmd status=%d, WIFEXITED(status)=%d, WEXITSTATUS(status)=%d\n", status, WIFEXITED(status), WEXITSTATUS(status));
    if(status != -1)
    {
        if(WEXITSTATUS(status) != 123)
        {
            printf("pclose: error\n");
            return -1;
        }
        else
        {
            printf("cmd exec success!\n");
        }
    }
    else
    {
        printf("cmd exec error\n");
    }
    return 0;
}
