#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 9001

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }
    
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(SERVER_PORT);

    int res = bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    char buf[512] = {0};
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t length = sizeof(client_addr);
    char IP_FOUND[] = "IP_FOUND";
    char IP_FOUND_ACK[] = "IP_FOUND_ACK";
    while(true)
    {
        memset(buf, 0, sizeof(buf));
        res = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &length);
        if(res <= 0)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            continue;
        }
        printf("recv msg: %s from %s:%d\n", buf, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if(strncmp(buf, IP_FOUND, strlen(IP_FOUND)) == 0)
        {
            res = sendto(sock, IP_FOUND_ACK, strlen(IP_FOUND_ACK), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            if(res <= 0)
            {
                printf("sendto error: %s\n", strerror(errno));
                continue;
            }
            printf("send ack success.\n");
        }
    }

    close(sock);

    return 0;
}
