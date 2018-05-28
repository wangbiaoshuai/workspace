#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define SERVER_IP "192.168.133.93"
#define SERVER_PORT 8087 

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("create sock error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);
    
    while(true)
    {
        int res = connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
        if(res < 0)
        {
            printf("connect %s:%d error: %s. Sleep 3S...\n", SERVER_IP, SERVER_PORT, strerror(errno));
            sleep(3);
        }
        else
        {
            printf("client: connect %s:%d success.\n", SERVER_IP, SERVER_PORT);
            break;
        }
    }

    while(true)
    {
        printf("begin send.\n");
        char buf[] = "Hello, server!";
        int res = send(sock, buf, strlen(buf), 0);
        sleep(1);

        char msg[1024] = {0};
        res = recv(sock, msg, sizeof(msg), 0);
        if(res > 0)
        {
            printf("client: %s\n", msg);
        }
        printf("recv: %s\n", msg);
        sleep(2);
    }

    close(sock);
    return 0;
}
