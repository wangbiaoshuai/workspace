#include <netdb.h> //getprotobyname
#include <string.h>
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h> //inet_addr
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h> //gettimeofday
#include <pthread.h>

using namespace std;
#define PACK_SIZE 1024

/*int MakeIcmpPack();
int SendIcmpPack();
int RecvIcmpPack();
int ParseIcmpPack();*/

typedef struct
{
    int sock;
    std::string dst_ip;
} Param;

static Param param;

void* RecvIcmpPack(void* param)
{
    int sock = ((Param*)param)->sock;
    string dst_ip = ((Param*)param)->dst_ip;

    char buf[PACK_SIZE] = {0};
    struct sockaddr_in from_addr;
    socklen_t len = sizeof(from_addr);
    int res = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&from_addr, &len);
    if(res < 0)
    {
        printf("RecvIcmpPack: recv error(%s)\n", strerror(errno));
        pthread_exit(NULL);
    }
    else
    {
        string dst_ip = inet_ntoa(from_addr.sin_addr);
        printf("RecvIcmpPack: recv message from %s\n", dst_ip.c_str());
    }
    pthread_exit(NULL);
}

int SendIcmpPack(int sock, const std::string& dst_ip, struct icmp* icmp_pack)
{
    struct sockaddr_in dst_addr;
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());

    int res = sendto(sock, icmp_pack, sizeof(struct icmp), 0, (struct sockaddr*)&dst_addr, sizeof(dst_addr));
    if(res < 0)
    {
        printf("SendIcmpPack: send msg error(%s)\n", strerror(errno));
        return -1;
    }
    printf("send pack success.\n");
    return 0;
}

unsigned short CheckSum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }
    if(nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

int MakeIcmpPack(struct icmp* pack)
{
    if(pack == NULL)
    {
        return -1;
    }

    pack->icmp_type = 8;//ICMP_ECHO;
    pack->icmp_code = 0;
    pack->icmp_cksum = 0;
    pack->icmp_id = htons(getpid());
    pack->icmp_seq = htons(1);
    gettimeofday((struct timeval*)pack->icmp_data, NULL);

    pack->icmp_cksum = CheckSum((unsigned short *)pack, sizeof(struct icmp));

    return 0;
}

pthread_t CreateRecvThread(int sock, const std::string& dst_ip)
{
    param.sock = sock;
    param.dst_ip = dst_ip;
    pthread_t recv_thread;
    int res = pthread_create(&recv_thread, NULL, RecvIcmpPack, &param);
    if(res < 0)
    {
        printf("CreateRecvThread: create thread error(%s)\n", strerror(errno));
        return 0;
    }

    return recv_thread;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: ping 172.16.18.41\n");
        return -1;
    }
    string dst_ip = argv[1];

    struct protoent* proto = getprotobyname("icmp");
    if(proto == NULL)
    {
        printf("getprotobyname error: %s\n", strerror(errno));
        return -1;
    }

    int sock = socket(AF_INET, SOCK_RAW, proto->p_proto);
    if(sock < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    struct icmp icmp_pack;
    memset(&icmp_pack, 0, sizeof(icmp_pack));
    MakeIcmpPack(&icmp_pack);
    pthread_t recv_thread = CreateRecvThread(sock, dst_ip);
    if(recv_thread <= 0)
    {
        close(sock);
        printf("CreateRecvThread failed.\n");
        return -1;
    }

    for(int i = 0; i < 10; i++)
    {
        int res = SendIcmpPack(sock, dst_ip, &icmp_pack);
        if(res < 0)
        {
            close(sock);
            printf("SendIcmpPack failed.\n");
            return -1;
        }
        sleep(1);
    }

    int res = pthread_join(recv_thread, NULL);
    if(res < 0)
    {
        close(sock);
        printf("pthread_join error: %s\n", strerror(errno));
        return -1;
    }
    close(sock);
    return 0;
}
