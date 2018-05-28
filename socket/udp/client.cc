#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "192.168.133.93"
#define SERVER_PORT 8087 

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    char buf[] = "Hello, server!";
    int res = sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("send error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in from;
    memset(&from, 0, sizeof(from));
    char msg[1024] = {0};
    int addr_len = sizeof(struct sockaddr);
    res = recvfrom(sock, msg, sizeof(msg), 0, (struct sockaddr*)&from, (socklen_t*)&addr_len);
    if(res < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        return -1;
    }

    printf("client: recv from %s:%d msg=%s\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), msg);

    close(sock);
    return 0;
}
