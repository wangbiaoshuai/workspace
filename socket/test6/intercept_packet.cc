#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <linux/ip.h>
#include <netinet/tcp.h>
#include <linux/udp.h>

int main()
{
    int sock = socket(AF_INET, SOCK_PACKET, htons(0x0003));
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    //设置网卡模式为混杂模式
    /*const char * eth_name = "eth0";
    struct ifreq ifr;
    strcpy(ifr.ifr_name, eth_name);

    int res = ioctl(sock, SIOCGIFFLAGS, &ifr);
    if(res < 0)
    {
        printf("ioctl get ifr error: %s\n", strerror(errno));
        return -1;
    }
    ifr.ifr_flags |= IFF_PROMISC;
    res = ioctl(sock, SIOCSIFFLAGS, &ifr);
    if(res < 0)
    {
        printf("ioctl set ifr error: %s\n", strerror(errno));
        return -1;
    }*/
    //设置完成
    
    //获取链路层数据
    char data[ETH_FRAME_LEN] = {0};
    int n = read(sock, data, sizeof(data));
    if(n < 0)
    {
        printf("read data error: %s\n", strerror(errno));
        return -1;
    }
    //获取完成
    
    //解析链路层数据
    struct ethhdr* p_ethhdr;
    p_ethhdr = (struct ethhdr*)data;
    printf("dest MAC: ");
    for(int i = 0; i < ETH_ALEN - 1; i++)
    {
        printf("%02X-", p_ethhdr->h_dest[i]);
    }
    printf("%02X\n", p_ethhdr->h_dest[ETH_ALEN - 1]);

    printf("src MAC: ");
    for(int i = 0; i < ETH_ALEN - 1; i++)
    {
        printf("%02X-", p_ethhdr->h_source[i]);
    }
    printf("%02X\n", p_ethhdr->h_source[ETH_ALEN - 1]);
    printf("protocol: 0x%04X\n", htons(p_ethhdr->h_proto));

    //解析IP报文
    if(htons(p_ethhdr->h_proto) == 0x0800)
    {
        struct iphdr * p_iphdr;
        p_iphdr = (struct iphdr*)(data + ETH_HLEN);
        struct in_addr addr;
        addr.s_addr = p_iphdr->saddr;
        printf("src ip: %s\n", inet_ntoa(addr));
        addr.s_addr = p_iphdr->daddr;
        printf("dst ip: %s\n", inet_ntoa(addr));
        printf("ihl: %d\n", p_iphdr->ihl);


        //获取TCP报文
        if(p_iphdr->protocol == 6)
        {
            struct tcphdr * p_tcphdr = (struct tcphdr*)(data + ETH_HLEN + p_iphdr->ihl * 4);
            printf("protocol: tcp\n");
            printf("src port: %d\n", htons(p_tcphdr->source));
            printf("dst port: %d\n", htons(p_tcphdr->dest));

            //获取应用层数据部分
            char* app_data = data + ETH_HLEN + 20 + 20;
            int app_data_len = n - ETH_HLEN - 20 - 20;
            printf("length: %d, app data: %s\n", app_data_len, app_data);
        }
        //获取udp报文
        else if(p_iphdr->protocol == 17)
        {
            struct udphdr* p_udphdr = (struct udphdr*)(data + ETH_HLEN + 20);
            printf("protocol: udp\n");
            printf("src port: %d\n", htons(p_udphdr->source));
            printf("dest port: %d\n", htons(p_udphdr->dest));
            printf("udp_header_len: %d\n", htons(p_udphdr->len));

            //获取应用层数据部分
            char* app_data = data + ETH_HLEN + 20 + 8;
            int app_data_len = n - ETH_HLEN - 20 - 8;
            printf("length: %d, app data: %s\n", app_data_len, app_data);
        }
    }

    close(sock);

    return 0;
}
