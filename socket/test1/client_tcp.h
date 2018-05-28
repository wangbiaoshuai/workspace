#ifndef _CLIENT_TCP_H
#define _CLIENT_TCP_H

#include <string>
#include <netinet/in.h>

class Client
{
public:
    Client();
    ~Client();
    int Init(const std::string& server_ip, int server_port);
    int AddRpc(int x, int y, int& result);
    int AddRpc(double x, double y, double& result);

private:
    int Open();
    void Close();

    int sock_;
    struct sockaddr_in server_addr_;
};
#endif // _CLIENT_TCP_H
