#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MCAST_IP "224.0.0.88"
#define MCAST_PORT 8888

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in multicast_addr;
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(MCAST_IP);
    multicast_addr.sin_port = htons(MCAST_PORT);


    char buf[] = "This is multicast test.";
    while(1)
    {
        int res = sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
        if(res <= 0)
        {
            printf("sendto error: %s\n", strerror(errno));
        }
        sleep(2);
    }

    close(sock);
    return 0;
}
