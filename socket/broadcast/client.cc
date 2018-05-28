#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#define IFNAME "eth0"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define SERVER_PORT 9001
#define CLIENT_PORT 9101

int main(void)
{
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    int flag = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));
    if(ret < 0)
    {
        printf("setsockopt error: %s\n", strerror(errno));
        return -1;
    }

    struct timeval recv_timeout = {5, 0};
    ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&recv_timeout, sizeof(recv_timeout));
    if(ret == -1)
    {
        printf("setsockopt recv_timeout error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(CLIENT_PORT);
    if(bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in from_addr;
    struct sockaddr_in broadcast_addr;
    struct ifreq ifr;
    strcpy(ifr.ifr_name, IFNAME);
    if(ioctl(sock, SIOCGIFBRDADDR, &ifr) < 0)
    {
        printf("get broadcast address error: %s\n", strerror(errno));
        return -1;
    }
    memcpy(&broadcast_addr, &(ifr.ifr_broadaddr), sizeof(broadcast_addr));
    broadcast_addr.sin_port = htons(SERVER_PORT);
    printf("broadcast address: %s\n", inet_ntoa(broadcast_addr.sin_addr));

    printf("begin send msg.\n");

    char buf[] = "IP_FOUND";
    for(int i = 0; i < 10; i++)
    {
        int ret = sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        if(ret <= 0)
        {
            printf("send msg failed: %s\n", strerror(errno));
            continue;
        }
        printf("send end.\n");
        
        char msg[20] = {0};
        socklen_t length = sizeof(from_addr);
        ret = recvfrom(sock, msg, sizeof(msg), 0, (struct sockaddr*)&from_addr, &length);
        if(ret <= 0)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            continue;
        }
        printf("recv: %s\n", msg);
        if(strncmp(msg, IP_FOUND_ACK, strlen(IP_FOUND_ACK)) == 0)
        {
            printf("server ip: %s\n", inet_ntoa(from_addr.sin_addr));
            break;
        }
    }
    close(sock);
    return 0;
}
