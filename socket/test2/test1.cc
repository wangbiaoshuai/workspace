#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

int main(int args, char* argv[])
{
    char name[30] = {0};
    int res = gethostname(name, sizeof(name));
    if(res < 0)
    {
        printf("gethostname error: %s\n", strerror(errno));
        return -1;
    }
    printf("hostname: %s\n", name);

    if(args != 2)
    {
        printf("Usage: ./a.out www.baidu.com\n");
        return -1;
    }

    char* name_1 = argv[1];
    struct hostent* host;
    host = gethostbyname(name_1);
    if(host == NULL)
    {
        herror("gethostbyname");
        return -1;
    }

    printf("hostname: %s\n", host->h_name);
    char** pptr = NULL;
    for(pptr = host->h_addr_list; *pptr != NULL; ++pptr)
        printf("ip addr: %s\n", inet_ntoa(*(struct in_addr*)(*pptr)));
    if(host->h_addrtype == AF_INET)
    {
        printf("addrtype: AF_INET\n");
    }
    if(host->h_addrtype == AF_INET6)
    {
        printf("addrtype: AF_INET6\n");
    }
    return 0;
}
