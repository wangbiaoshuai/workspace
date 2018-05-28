#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "client_tcp.h"

#define SERVER_IP "192.168.133.93"
#define SERVER_PORT 8087

Client::Client() :
sock_(0)
{
}

Client::~Client()
{
}

int Client::Init(const std::string& server_ip, int server_port)
{
    memset(&server_addr_, 0, sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server_addr_.sin_port = htons(server_port);

    return 0;
}

void Client::Close()
{
    close(sock_);
}

int Client::Open()
{
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_ < 0)
    {
        printf("create sock error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int Client::AddRpc(int x, int y, int& result)
{
    if(Open() < 0)
        return -1;

    int res = connect(sock_, (struct sockaddr*)&server_addr_, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("AddRpc int: connect error(%s)\n", strerror(errno));
        return -1;
    }

    char msg[30] = {0};
    sprintf(msg, "add_int:%d:%d", x, y);
    res = send(sock_, msg, strlen(msg), 0);
    if(res < 0)
    {
        printf("AddRpc: send error(%s)\n", strerror(errno));
        return -1;
    }

    char ret[10] = {0};
    res = recv(sock_, ret, sizeof(ret), 0);
    if(res < 0)
    {
        printf("AddRpc: recv error(%s)\n", strerror(errno));
        return -1;
    }

    result = atoi(ret);
    Close();
    return 0;
}

int Client::AddRpc(double x, double y, double& result)
{
    if(Open() < 0)
        return -1;
    int res = connect(sock_, (struct sockaddr*)&server_addr_, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("AddRpc double: connect error(%s)\n", strerror(errno));
        return -1;
    }

    char msg[50] = {0};
    sprintf(msg, "add_double:%lf:%lf", x, y);
    res = send(sock_, msg, strlen(msg), 0);
    if(res < 0)
    {
        printf("AddRpc: send error(%s)\n", strerror(errno));
        return -1;
    }

    char ret[20] = {0};
    res = recv(sock_, ret, sizeof(ret), 0);
    if(res < 0)
    {
        printf("AddRpc: recv error(%s)\n", strerror(errno));
        return -1;
    }

    result = atof(ret);
    Close();
    return 0;
}

/*int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
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

    int res = connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    if(res < 0)
    {
        printf("connect error: %s\n", strerror(errno));
        return -1;
    }

    char buf[] = "add:10:31";
    res = send(sock, buf, strlen(buf), 0);
    if(res < 0)
    {
        printf("send error: %s\n", strerror(errno));
        return -1;
    }

    char result[512] = {0};
    res = recv(sock, result, sizeof(result), 0);
    if(res < 0)
    {
        printf("recv error: %s\n", strerror(errno));
        return -1;
    }
    close(sock);

    printf("%s=%s\n", buf, result);
    return 0;
}*/
