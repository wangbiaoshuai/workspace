#include "client_tcp.h"

#include <stdio.h>

#define SERVER_IP "192.168.133.93"
#define SERVER_PORT 8087

int main()
{
    Client client;
    int res = client.Init(SERVER_IP, SERVER_PORT);
    if(res < 0)
    {
        return -1;
    }

    int result;
    res = client.AddRpc(108, 676, result);
    if(res < 0)
        return -1;
    printf("108+676=%d\n", result);

    double result_d;
    res = client.AddRpc(123.456, 678.1254, result_d);
    if(res < 0)
        return -1;
    printf("123.456+678.1254=%lf\n", result_d);

    return 0;
}
