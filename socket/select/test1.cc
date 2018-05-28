#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define STDIN 0

int main()
{
    struct timeval tv;

    fd_set rset;

    while(true)
    {
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        FD_ZERO(&rset);
        FD_SET(STDIN, &rset);

        int res = select(STDIN + 1, &rset, NULL, NULL, &tv);
        if(res < 0)
        {
            printf("select error: %s\n", strerror(errno));
            return -1;
        }
        else if(res == 0)
        {
            printf("Time out.\n");
        }

        if(FD_ISSET(STDIN, &rset))
        {
            printf("a key has pressed.\n");
            break;
        }
    }
    return 0;
}
