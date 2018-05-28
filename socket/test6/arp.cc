#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define DEST_ADDR "192.168.133.150"

struct arppacket
{
    unsigned short ar_hrd;
    unsigned short ar_pro;
    unsigned char ar_hln;
    unsigned char ar_pln;
    unsigned short ar_op;
    unsigned char ar_sha[ETH_ALEN];
    uint32_t ar_sip;
    unsigned char ar_tha[ETH_ALEN];
    uint32_t ar_tip;
};

int main()
{
    printf("unsigned long:%d, unsigned short:%d\n", sizeof(unsigned long), sizeof(unsigned short));
    char ef[ETH_FRAME_LEN] = {0};

    //目的硬件地址
    char eth_dest[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    //源硬件地址
    char eth_source[ETH_ALEN] = {0x00, 0x0C, 0x29, 0xA2, 0x00, 0x89};

    //填充eth头
    struct ethhdr* p_ethhdr = (struct ethhdr*)ef;
    memcpy(p_ethhdr->h_dest, eth_dest, ETH_ALEN);
    memcpy(p_ethhdr->h_source, eth_source, ETH_ALEN);
    p_ethhdr->h_proto = htons(0x0806);

    //填充arp协议
    struct arppacket* p_arppacket = (struct arppacket*)(ef + 14);
    p_arppacket->ar_hrd = htons(0x01);
    p_arppacket->ar_pro = htons(0x0800);
    p_arppacket->ar_hln = 6;
    p_arppacket->ar_pln = 4;
    p_arppacket->ar_op = htons(0x01);
    memcpy(p_arppacket->ar_sha, eth_source, ETH_ALEN);
    p_arppacket->ar_sip = inet_addr("192.168.133.93");
    //memcpy(p_arppacket->ar_tha, eth_dest, ETH_ALEN);
    memset(p_arppacket->ar_tha, 0, ETH_ALEN);
    uint32_t dest_ip = inet_addr(DEST_ADDR);
    p_arppacket->ar_tip = dest_ip;

    int sock = socket(AF_INET, SOCK_PACKET, htons(0x0806));
    if(sock < 0)
    {
        printf("create socket error: %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr sa;
    strcpy(sa.sa_data, "eth0");
    while(true)
    {
        int n = sendto(sock, ef, sizeof(ef), 0, &sa, sizeof(sa));

        // int n = write(sock, ef, sizeof(ef));
        if(n <= 0)
        {
            printf("write socket error: %s\n", strerror(errno));
            return -1;
        }
        sleep(5);
    }

/*    while(true)
    {
        memset(ef, 0, sizeof(ef));
        n = read(sock, ef, sizeof(ef));
        if(n <= 0)
        {
            printf("read error: %s\n", strerror(errno));
            return -1;
        }

        //解析eth头
        p_ethhdr = (struct ethhdr*)ef;
        if(p_ethhdr->h_proto != 0x0806)
        {
            printf("not arp.\n");
            continue;
        }

        //解析arp包
        p_arppacket = (struct arppacket*)(ef + 14);

        if(p_arppacket->ar_sip != dest_ip)
        {
            printf("not dest ip.\n");
            continue;
        }

        printf("source mac: ");
        for(int i = 0; i < 5; i++)
        {
            printf("%02x:", p_arppacket->ar_sha[i]);
        }
        printf("%02x\n", p_arppacket->ar_sha[5]);

        printf("dest mac: ");
        for(int i = 0; i < 5; i++)
        {
            printf("%02x:", p_arppacket->ar_tha[i]);
        }
        printf("%02x\n", p_arppacket->ar_tha[5]);
        break;
    }*/

    close(sock);
    return 0;
}
