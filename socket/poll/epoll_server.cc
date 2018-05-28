#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "thread_pool.h"

#define SERVER_PORT 8888
#define WAIT_Q 10
#define EVENT_NUM 64

int init_socket()
{
    int s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(s_sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(s_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    res = listen(s_sock, WAIT_Q);
    if(res < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }
    return s_sock;
}

void* process_events(void* p_sock)
{
    int sock = *((int*)p_sock);
    char buf[1024] = {0};
    int res = recv(sock, buf, sizeof(buf) - 1, 0);
    if(res < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        close(sock);
        return NULL;
    }
    else if(res == 0)
    {
        printf("client socket closed\n");
        close(sock);
        return NULL;
    }
    sprintf(buf, "%d charactor recved\n", res);
    sleep(10);
    res = send(sock, buf, strlen(buf), 0);
    if(res < 0)
    {
        printf("send error: %s\n", strerror(errno));
        close(sock);
        return NULL;
    }
    else if(res == 0)
    {
        printf("client socket closed\n");
        close(sock);
        return NULL;
    }
    return NULL;
}

void set_nonblock(int sock)
{
    int fl = fcntl(sock, F_GETFL);
    fcntl(sock, F_SETFL, fl | O_NONBLOCK);
}

int main()
{
    int listen_sock = init_socket();
    if(listen_sock < 0)
    {
        printf("init_socket failed.\n");
        return -1;
    }
    set_nonblock(listen_sock);

    int epfd = epoll_create(1024);
    if(epfd < 0)
    {
        printf("epoll_create error: %s\n", strerror(errno));
        return -1;
    }

    struct epoll_event event;
    struct epoll_event evs[EVENT_NUM];
    event.events = EPOLLIN;
    event.data.fd = listen_sock;

    int res = epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &event);
    if(res < 0)
    {
        printf("epoll_ctl error: %s\n", strerror(errno));
        return -1;
    }

    ThreadPool thread_pool;
    if(thread_pool.Init(5) < 0)
    {
        printf("init thread pool failed\n");
        return -1;
    }

    while(true)
    {
        res = epoll_wait(epfd, evs, EVENT_NUM, -1);
        if(res < 0)
        {
            printf("epoll_wait error: %s\n", strerror(errno));
            continue;
        }
        else if(res == 0)
        {
            printf("time out\n");
            continue;
        }
        else
        {
            for(int i = 0; i < res; i++)
            {
                int rsock = evs[i].data.fd;
                if(rsock == listen_sock && (evs[i].events & EPOLLIN))
                {
                    struct sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &len);
                    if(client_sock < 0)
                    {
                        printf("accept error: %s\n", strerror(errno));
                        break;
                    }
                    set_nonblock(client_sock);
                    event.data.fd = client_sock;
                    event.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
                }
                else if(evs[i].events & EPOLLIN)
                {
                    thread_pool.AddJob(process_events, &rsock);
                    /*if(process_events(rsock) < 0)
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, rsock, NULL);
                    }*/
                }
            }
        }
    }

    close(epfd);
    close(listen_sock);
    thread_pool.Destroy();
    return 0;
}
