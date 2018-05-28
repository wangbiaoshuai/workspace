#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERVER_PORT 8888
#define SERVER_IP "192.168.133.93"

int proc_conn_client(int sock)
{
    char buf[1024];
    while(true)
    {
        memset(buf, 0, sizeof(buf));
        int res = read(0, buf, sizeof(buf) - 1);
        if(res <= 0)
        {
            printf("proc_conn_client: read error(%s)\n", strerror(errno));
            return -1;
        }

        res = send(sock, buf, strlen(buf), 0);
        if(res <= 0)
        {
            printf("proc_conn_client: send error(%s)\n", strerror(errno));
            return -1;
        }

        memset(buf, 0, sizeof(buf));
        res = recv(sock, buf, sizeof(buf) - 1, 0);
        if(res <= 0)
        {
            printf("proc_conn_client: recv error(%s)\n", strerror(errno));
            return -1;
        }

        res = write(1, buf, strlen(buf));
        if(res <= 0)
        {
            printf("proc_conn_client: write error(%s)\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

void my_sigpipe_handler(int signal)
{
    printf("Catch a SIGPIPE signal.\n");
}

int main()
{
    signal(SIGPIPE, my_sigpipe_handler);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int res = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(res < 0)
    {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }

    res = proc_conn_client(sock);
    if(res < 0)
    {
        printf("proc_conn_client failed.\n");
    }
    close(sock);
    return 0;
}
