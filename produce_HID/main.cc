#include <stdio.h> 
#include <sys/ioctl.h> 
#include <linux/hdreg.h> 
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include "md5module.h"

using namespace std;
#define HD_DEVICE "eth0"

string get_disk_serial() 
{
    string hd_serial("");
    struct hd_driveid id;
    memset(&id, 0, sizeof(struct hd_driveid));
    int fd = open("/dev/sda", O_RDONLY|O_NONBLOCK); 

    if (fd < 0) 
    {
        printf("open:%s\n", strerror(errno));
        return hd_serial;
    } 
    //调用ioctl() 
    if(!ioctl(fd, HDIO_GET_IDENTITY, &id)) 
    { 
        hd_serial = (const char*)id.serial_no;
        printf("Serial Number=%s\n",hd_serial.c_str());
    }
    else
    {
        printf("ioctl:%s\n", strerror(errno));
    }
    close(fd);
    return hd_serial;
}

unsigned int veax;
unsigned int vebx;
unsigned int vedx;
unsigned int vecx;
//执行CPUID指令
void cpuid(unsigned int veax1)
{
    asm("cpuid"
        :"=a"(veax),
        "=b"(vebx),
        "=c"(vecx),
        "=d"(vedx)
        :"a"(veax));
}
//做移位操作，把寄存器中的值以“%d”形式输出
void LM(unsigned int var, uint32_t *vx)
{
    int i;
    for(i=0;i<3;i++)
    {
        var=(var>>i);
        vx[i]=var;
    }
}

static void getcpuid (char *id)
{
    uint32_t ax[3],cx[3],dx[3];
    cpuid(1);
    LM(veax,ax);
    cpuid(3);
    LM(vecx,cx);
    LM(vedx,dx);
    sprintf(id,"%u%u%u%u%u%u%u%u%u",ax[0],ax[1],ax[2],cx[0],cx[1],cx[2],dx[0],dx[1],dx[2]);
}

string get_cpu_serial()
{
    string cpu_serial("");
    char cpuid[30];
    getcpuid(cpuid);
    cpu_serial = cpuid;
    return cpu_serial;
}

string get_mac_serial()
{
    string mac_serial("");
    struct ifreq ifreq;
    int sock = 0;
    char mac[32] = "";

    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("error sock");
        return mac_serial;
    }

    strcpy(ifreq.ifr_name,HD_DEVICE);
    //ifreq.ifr_name = HD_DEVICE;
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) < 0)
    {
        perror("error ioctl");
        return mac_serial;
    }

    int i = 0;
    for(i = 0; i < 6; i++){
        sprintf(mac+3*i, "%02X:", (unsigned char)ifreq.ifr_hwaddr.sa_data[i]);
    }
    mac[strlen(mac) - 1] = 0;
    mac_serial = mac;
                
    return mac_serial;
}

bool is_space(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

void trim(string & str)
{
    if (str.empty()) {
        return;
    }
    unsigned int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!is_space(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }

    start_pos = i;

    for (i = str.size() - 1; i >= start_pos; --i) {
        if (!is_space(str[i])) {
            break;
        }
    }
    end_pos = i;

    str = str.substr(start_pos, end_pos - start_pos + 1);
}

void  itoa(unsigned long val,  char *buf, unsigned radix)
{
    char   *p;
    char   *firstdig;
    char   temp;
    unsigned   digval;

    p   =   buf;
    firstdig   =   p;

    do{
        digval =  (unsigned)(val % radix);
        val /=  radix;
        if(digval  >  9)
            *p++  = (char)(digval - 10  + 'a');
        else
            *p++ = (char)(digval + '0');
    } while (val > 0);

    *p-- = '\0';

    do{
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;
    }while(firstdig < p);
}

std::string GetStrMd5(const char* szSrc, int iLen)
{
    unsigned char szmd5sum[16];
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, (unsigned char *)szSrc, iLen);
    md5_finish(&ctx, szmd5sum);

    char szMd5[260] = {0};
    char szChar[3];
    for(int i = 0; i < 16; i ++)
    {
        itoa(szmd5sum[i], szChar, 16);
        if(strlen(szChar) == 1)
        {
            strcat((char*)szMd5,  "0");
        }
        strcat((char*)szMd5,  szChar);
    }

    std::string szResult = szMd5;

    return szResult;
}

int main(void)
{
    string value("");
    string hd_serial = get_disk_serial();
    string cpu_serial = get_cpu_serial();
    string mac_serial = get_mac_serial();
    value = hd_serial + cpu_serial + mac_serial;
    trim(value);

    string key = GetStrMd5(value.c_str(), value.length());
    printf("value:%s\nkey:%s\n", value.c_str(), key.c_str());
    return 0;
}
