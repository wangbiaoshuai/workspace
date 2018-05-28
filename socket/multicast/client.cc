#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MCAST_PORT 8888
#define MCAST_IP "224.0.0.88"

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in local_addr;
    struct sockaddr_in multicast_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(MCAST_PORT);


    int res = bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    int loop = 1;
    res = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if(res < 0)
    {
        printf("setsockopt loop error: %s\n", strerror(errno));
        return -1;
    }

    struct ip_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_IP);
    mreq.imr_interface.s_addr = INADDR_ANY;
    res = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if(res < 0)
    {
        printf("setsockopt add broadcast error: %s\n", strerror(errno));
        return -1;
    }

    char buf[100] = {0};
    for(int i = 0; i < 4; i++)
    {
        struct sockaddr_in tmp_addr;
        socklen_t length = sizeof(tmp_addr);
        memset(&tmp_addr, 0, length);
        res = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&tmp_addr, &length);
        if(res <= 0)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            break;
        }
        res < 100? buf[res] = 0 : buf[99] = 0;

        printf("recv msg--%s from addr--%s:%d\n", buf, inet_ntoa(tmp_addr.sin_addr), ntohs(tmp_addr.sin_port));

        sleep(3);
    }


    setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sock);
    return 0;
}
