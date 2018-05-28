#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 100   //最大长度
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 8087
#define INFTIM 1000
#define IP_ADDR "192.168.133.93"

int main()
{
    struct epoll_event ev, events[20];
    struct sockaddr_in clientaddr, serveraddr;
    int epfd;
    int listenfd;//监听fd
    int maxi;
    int nfds;
    int i;
    int sock_fd, conn_fd;
    char buf[MAXLINE] = {0};

    epfd = epoll_create(256);//生成epoll句柄
    listenfd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字
    ev.data.fd = listenfd;//设置与要处理事件相关的文件描述符
    ev.events = EPOLLIN;//设置要处理的事件类型

    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);//注册epoll事件

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERV_PORT);
    bind(listenfd,(struct sockaddr*)&serveraddr, sizeof(serveraddr));//绑定套接口
    socklen_t clilen;
    listen(listenfd, LISTENQ);//转为监听套接字
    int n;
    while(1)
    {
        printf("begin epoll_wait.\n");
        nfds = epoll_wait(epfd,events,20,-1);//等待事件发生
        //处理所发生的所有事件
        if(nfds < 0)
        {
            printf("epoll_wait error: %s\n", strerror(errno));
            break;
        }
        else if(nfds == 0)
        {
            printf("epoll_wait timeout.\n");
            continue;
        }
        else
        {
            for(i=0;i<nfds;i++)
            {
                if(events[i].data.fd == listenfd && (events[i].events & EPOLLIN))//有新的连接
                {
                    clilen = sizeof(struct sockaddr_in);
                    conn_fd = accept(listenfd, (struct sockaddr*)&clientaddr, &clilen);
                    printf("accept a new client : %s\n",inet_ntoa(clientaddr.sin_addr));
                    ev.data.fd = conn_fd;
                    ev.events = EPOLLIN;//设置监听事件为可写
                    epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);//新增套接字
                }
                else if(events[i].events & EPOLLIN)//可读事件
                {
                    sock_fd = events[i].data.fd;
                    if(sock_fd < 0)
                        continue;
                    memset(buf, MAXLINE, 0);
                    if((n = recv(sock_fd, buf, MAXLINE, 0)) < 0)
                    {
                        if(errno == ECONNRESET)
                        {
                            close(sock_fd);
                            printf("close client socket.\n");
                            //events[i].data.fd = -1;
                        }
                        else
                        {
                            printf("readline error\n");
                        }
                    }
                    else if(n == 0)
                    {
                        close(sock_fd);
                        printf("关闭\n");
                        //events[i].data.fd = -1;
                    }

                    printf("%d -- > %s\n",sock_fd, buf);
                    ev.data.fd = sock_fd;
                    ev.events = EPOLLOUT;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,sock_fd,&ev);//修改监听事件为可读
                }

                else if(events[i].events & EPOLLOUT)//可写事件
                {
                    sock_fd = events[i].data.fd;
                    printf("OUT\n");
                    scanf("%s",buf);
                    send(sock_fd, buf, MAXLINE, 0);

                    ev.data.fd = sock_fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_MOD,sock_fd, &ev);
                }
            }
        }
    }

    close(listenfd);
    close(epfd);

    return 0;   
}
