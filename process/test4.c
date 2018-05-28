#include <stdlib.h>
#include <string>
#include <stdio.h>

using namespace std;

int main()
{
    string cmd = "./test3.sh";
    int status = system(cmd.c_str());
    printf("main: WIFEXITED(status)=%d, WEXITSTATUS(status)=%d\n", WIFEXITED(status), WEXITSTATUS(status));
    if(WIFEXITED(status) != 0)
    {
        if(WEXITSTATUS(status) != 0)
        {
            printf("cmd exec error. WEXITSTATUS(status)=%d\n", WEXITSTATUS(status));
            return -1;
        }
        else
        {
            printf("cmd exec success!\n");
            return 0;
        }
    }
    else
    {
        printf("cmd exit error\n");
        return -1;
    }
    return 0;
}
