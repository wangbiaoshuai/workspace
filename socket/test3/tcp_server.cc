#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8888
#define WAIT_Q 10

int proc_conn_server(int sock)
{
    struct timeval timeout;
    timeout.tv_sec = 20;
    timeout.tv_usec = 0;
    int res = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    if(res < 0)
    {
        printf("setsockopt send timeout error: %s\n", strerror(errno));
        return -1;
    }
    res = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    if(res < 0)
    {
        printf("setsockopt recv timeout error: %s\n", strerror(errno));
        return -1;
    }
    char buf[1024];
    while(true)
    {
        memset(buf, 0, sizeof(buf));
        int res = recv(sock, buf, sizeof(buf) - 1, 0);
        if(res <= 0)
        {
            printf("read error: %s\n", strerror(errno));
            return -1;
        }
        sprintf(buf, "%d bytes altogether.\n", res);
        res = send(sock, buf, strlen(buf), 0);
        if(res <= 0)
        {
            printf("write error: %s\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}

void my_sigchld_handler(int sig)
{
    if(wait(NULL) < 0)
    {
        printf("wait error: %s\n", strerror(errno));
    }
}

int main()
{
    signal(SIGCHLD, my_sigchld_handler);
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    int reuse = 1;
    int res = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    res = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    res = listen(server_sock, WAIT_Q);
    if(res < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }

    while(true)
    {
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &len);
        if(client_sock < 0)
        {
            printf("accept error: %s\n", strerror(errno));
            continue;
        }
        int pid = fork();
        if(pid == 0)
        {
            close(server_sock);
            int res = proc_conn_server(client_sock);
            if(res < 0)
            {
                printf("proc_conn_server failed.\n");
            }
            close(client_sock);
            exit(0);
        }
        else if(pid > 0)
        {
            close(client_sock);
        }
        else
        {
            printf("fork error.\n");
            break;
        }
    }

    close(server_sock);
    return 0;
}
