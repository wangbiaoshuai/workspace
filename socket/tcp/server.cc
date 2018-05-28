#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> // 声明结构体struct sockaddr_in 
#include <arpa/inet.h>  // 声明函数inet_ntoa()函数
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define PORT 8087

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    //设置地址可重用
    /*int flag = 1;
    int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(res < 0)
    {
        printf("setsockopt reuse address error: %s\n", strerror(errno));
    }*/

    int flag;
    socklen_t len = sizeof(flag);
    int res = getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, &len);
    if(res < 0)
    {
        printf("getsockopt reuse address error: %s\n", strerror(errno));
    }
    printf("flag: %d\n", flag);
    
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);
    bzero(&(my_addr.sin_zero), sizeof(my_addr.sin_zero));

    res = bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    res = listen(sock, 10);
    if(res < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in client_addr;
    int addr_len = sizeof(sockaddr); // 这一个很关键，如果不对其进行初始化，则最后存储的数据空间不够，client的ip值无法存入，打印不出来。
    int i = 0;
    while(1)
    {
        int client_sock = accept(sock, (sockaddr*)&client_addr, (socklen_t*)&addr_len);
        if(client_sock < 0)
        {
            printf("accept error: %s\n", strerror(errno));
            return -1;
        }

        printf("server: %s is connect!\n", inet_ntoa(client_addr.sin_addr));
        i++;

        char buf[1024] = {0};
        int res = recv(client_sock, buf, sizeof(buf), 0);
        printf("server: %s\n", buf);
        if(strcmp(buf, "Hello, server!") == 0)
        {
            char msg[512] = {0};
            sprintf(msg, "Hello, client-%d!", i);
            res = send(client_sock, msg, strlen(msg), 0); 
        }

        close(client_sock); // 不能忘记关机client_sock
    }

    close(sock);
    return 0;
}
