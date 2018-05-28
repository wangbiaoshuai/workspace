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

using namespace std;

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

    int res = SendIcmpPack(sock, dst_ip, &icmp_pack);
    if(res < 0)
    {
        close(sock);
        printf("SendIcmpPack failed.\n");
        return -1;
    }

    close(sock);
    return 0;
}
