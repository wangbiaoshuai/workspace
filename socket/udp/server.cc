#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8087

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
    my_addr.sin_port = htons(PORT);

    int res = bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    int i = 0;
    while(1)
    {
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        int addr_len = sizeof(struct sockaddr);
        char buf[1024] = {0};
        res = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if(res < 0)
        {
            printf("recvfrom error: %s\n", strerror(errno));
            return -1;
        }
        printf("server: recvfrom %s:%d msg=%s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
        
        if(strcmp(buf, "Hello, server!") == 0)
        {
            sleep(1);
            i++;
            char msg[512] = {0};
            sprintf(msg, "Hello, client-%d!", i);
            int res = sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&client_addr, sizeof(struct sockaddr));
        }
    }
    
    close(sock);
    return 0;
}
