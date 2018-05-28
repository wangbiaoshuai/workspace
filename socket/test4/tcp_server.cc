#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <algorithm>
#include "thread_pool.h"

using namespace std;

#define MY_PORT 8888
#define WAIT_NUM 10

static vector<int> close_fds;

void* proc_server_request(void* p_sock)
{
    int sock = *((int*)p_sock);
    char buf[1024] = {0};
    int size = recv(sock, buf, sizeof(buf) - 1, 0);
    if(size <= 0)
    {
        printf("recv error: %s\n", strerror(errno));
        close_fds.push_back(sock);
        close(sock);
        return NULL;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d bytes altogether.\n", size);
    sleep(10);
    size = send(sock, buf, strlen(buf), 0);
    if(size <= 0)
    {
        printf("send error: %s\n", strerror(errno));
        close_fds.push_back(sock);
        close(sock);
        return NULL;
    }
    return NULL;
}

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        printf("create sock error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(MY_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(res < 0)
    {
        printf("bind error: %s\n", strerror(errno));
        return -1;
    }

    res = listen(sock, WAIT_NUM);
    if(res < 0)
    {
        printf("listen error: %s\n", strerror(errno));
        return -1;
    }

    fd_set rfds, efds;
    int max_fds = sock;
    vector<int> array_fds;
    vector<int>::iterator iter;
    ThreadPool thread_pool;
    if(thread_pool.Init(10) < 0)
    {
        printf("thread pool init failed\n");
        return -1;
    }

    struct timeval traval;
    traval.tv_sec = 100 * 60;
    traval.tv_usec = 0;

    while(true)
    {
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        FD_ZERO(&efds);
        FD_SET(sock, &efds);
        max_fds = sock;
        iter = array_fds.begin();
        while(iter != array_fds.end())
        {
            vector<int>::iterator iter_close = find(close_fds.begin(), close_fds.end(), *iter);
            if(iter_close != close_fds.end())
            {
                array_fds.erase(iter);
                close_fds.erase(iter_close);
                continue;
            }
            FD_SET(*iter, &rfds);
            FD_SET(*iter, &efds);
            if(max_fds < *iter)
            {
                max_fds = *iter;
            }
            ++iter;
        }

        int res = select(max_fds + 1, &rfds, NULL, &efds, NULL);
        if(res < 0)
        {
            printf("select error: %s\n", strerror(errno));
            break;
        }
        else if(res == 0)
        {
            printf("select timeout\n");
            continue;
        }
        else
        {
            if(FD_ISSET(sock, &rfds))
            {
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int new_sock = accept(sock, (struct sockaddr*)&client_addr, &len);
                if(new_sock < 0)
                {
                    printf("accept error: %s\n", strerror(errno));
                    continue;
                }
                array_fds.push_back(new_sock);
                continue;
            }
            if(FD_ISSET(sock, &efds))
            {
                printf("server socket error: %s\n", strerror(errno));
                break;
            }

            iter = array_fds.begin();
            while(iter != array_fds.end())
            {
                if(FD_ISSET(*iter, &rfds))
                {
                    //res = proc_server_request(*iter);
                    printf("server add job...\n");
                    thread_pool.AddJob(proc_server_request, &(*iter));
                    usleep(20);  //等待数据被读取出来，否则下次循环select还是可以监视到该文件描述符的变化。
                    /*if(res < 0)
                    {
                        close(*iter);
                        array_fds.erase(iter);
                        printf("proc_server_request failed\n");
                        continue;
                    }*/
                }
                if(FD_ISSET(*iter, &efds))
                {
                    close(*iter);
                    array_fds.erase(iter);
                    printf("client sock error: %s\n", strerror(errno));
                    continue;
                }
                ++iter;
            }
        }
    }

    close(sock);
    for(iter = array_fds.begin(); iter != array_fds.end(); ++iter)
    {
        close(*iter);
    }
    
    thread_pool.Destroy();
    return 0;
}

