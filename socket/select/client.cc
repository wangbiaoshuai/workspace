#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
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

    /*struct timeval tv;
      tv.tv_sec = 3;
      tv.tv_usec = 0;

      if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
      {
      printf("setsockopt failed: %s\n", strerror(errno));
      return -1;
      }*/
    int flags = fcntl(sock, F_GETFL, 0);
    if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        printf("clent: fcntl failed.\n");
        return -1;
    }

    printf("begin connect\n");
    
    struct timeval tv;
    fd_set rset, wset;
    int error;
    int res = connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    if(res < 0)
    {
        if(errno != EINPROGRESS)
        {
            printf("client: connect error:%s.\n", strerror(errno));
            return -1;
        }
    }

    printf("connect end.\n");

    while(true)
    {
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_SET(sock, &rset);
        wset = rset;

        res = select(sock + 1, &rset, &wset, NULL, &tv);
        if(res < 0)
        {
            printf("client: select error:%s.\n", strerror(errno));
            return -1;
        }
        else if(res == 0)
        {
            printf("client: select timeout, sleep 3S...\n");
            sleep(3);
            continue;
        }
        if(FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset))
        {
            int len = sizeof(error);
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len) < 0) 
            {
                continue;
            }
            break;
        }
    }

    printf("select end.\n");
    fcntl(sock, F_SETFL, flags);

    char buf[] = "Hello, server!";
    res = send(sock, buf, strlen(buf), 0);
    if(res < 0)
    {
        printf("send error: %s\n", strerror(errno));
        return -1;
    }

    printf("send end.\n");

    char msg[1024] = {0};
    res = recv(sock, msg, sizeof(msg), 0);
    if(res > 0)
    {
        printf("client: %s\n", msg);
    }

    close(sock);
    return 0;
}
