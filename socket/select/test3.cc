#include <stdio.h>

int main(void)
{
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }


    return 0;
}
