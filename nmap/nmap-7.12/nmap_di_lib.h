#ifndef NMAP_DI_LIB_H
#define NMAP_DI_LIB_H

#include "dev_identify.h"
#include "linux_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NMAP_BIT_IP(x)   ((unsigned char*)&(x))
#define NMAP_STR_IP(x)   NMAP_BIT_IP(x)[0], NMAP_BIT_IP(x)[1], NMAP_BIT_IP(x)[2], NMAP_BIT_IP(x)[3]
#define NMAP_FMT_IP      "%d.%d.%d.%d"

#define DI_CONTENT_KEY_LEN 128
#define DI_CONTENT_HASH_SIZE 8192 //2^13
#define DI_KEY_TEXT 32
#define FILE_LINE_LEN 1024

#define NMAP_NAME_LEN 64
#define NMAP_PORT_COUNT 50

#define DI_CONTENT_FP_FILE "/usr/local/devscan/di_lib/content-fingerprint-db"
#define DI_TELNET_FP_FILE "/usr/local/devscan/di_lib/telnet-fingerprint-db"
#define DI_DEV_FP_FILE "/usr/local/devscan/di_lib/dev-fingerprint-db"
#define DI_DEVTYPE_FILE "/usr/local/devscan/di_lib/identify-device-type"

/*file:identify-systype-type*/
enum
{
	UNKNOW_OS = 0,
	ALL_OS,
	WINDONS,
	WINDONS_MOBILE,
	WINDONS_SERVER,
	LINUX_OS,
	ANDROID,
	CISCO_IOS,
	APPLE_IOS,
	MAC_OS_X,
	VXWORKS,
	EMBEDDED,
	HUAWEI_VRP,
	H3C_COMWARE,
	VMWARE_ESXI,
	FREEBSD,
	APC_AOS,
	SYSCLASS_NUM
};

/*file:identify-device-type*/
enum
{
	UNKNOW_DEV = 0, //未知设备
	ALL_DEV, //所有设备
	PC,             //个人电脑
	THIN_CLIENTS, //瘦客户端
	VMWARE, //虚拟机
	MOBILE,     //移动设备
	PRINTER,  //网络打印机
	IP_PHONE, //IP电话
	CAMERA,    //网络摄像机
	POS,    //POS机
	SWITCH, //交换机
	ROUTER, //路由器
	WIRELESS_ROUTER, //无线路由器
	WAP,      //无线AP
	NETDEV, //网络设备
	AD_SERVER, //AD服务器
	LDAP_SERVER, //LDAP服务器
	WINDOWS_SERVER, //WINDOWS服务器
	LINUX_SERVER, //Linux服务器
	VIDEO_STORAGE_SERVER, //视频存储服务器
	HARDDISK_VIDEO_RECORDER,//网络硬盘录像机
	CONSOLE_SERVER,//串口服务器
	FAX_MACHINE,//传真机
	SCANNER,//扫描仪
	ATTENDANCE_MACHINE,//考勤机
	ENTRANCE_GUARD,//门禁
	SSPI_TERMINAL,//自助缴费查询终端
	UPS,//UPS主机
	CAS_SYSTEM,//CAC在线监测装置系统
	TIME_ATTENDANCE,//考勤系统
	PEM_HOST,//动力环境监控主机
	POLYCOM,//Polycom终端
	IVC_TERMINAL,//一体化视频会议终端
	MCU_SERVER,//MCU服务器
	VCV_SERVER,//视频会议录播服务器
	VM_PLATFORM,//视频监控平台
	DVR,//数字硬盘录像机
	DNBZZG_SYSTEM,//电能表计量周转柜系统
	CLOUD_TERMINAL,//云终端
	NAS_SYSTEM,//NAS系统
	VMWARE_VSPHERE_PLATFORM,//vSphere虚拟化平台
	VMWARE_ESXI_PLATFORM,//ESXi虚拟化平台
	DEV_TYPE_MAX = 100
};


typedef struct di_content_fp_s di_content_fp_t;
typedef struct di_content_fp_node_s di_content_fp_node_t;
typedef struct di_telnet_fp_s di_telnet_fp_t;
typedef struct di_telnet_fp_node_s di_telnet_fp_node_t;

struct nmap_identify_s
{
	int result;
	int status;
	int devtype_flag;
	int systype_flag;
	int netbios;
	char devtype_name[NMAP_NAME_LEN];
	char systype_name[NMAP_NAME_LEN];
	char sysclass_name[NMAP_NAME_LEN];
	char dev_name[NMAP_NAME_LEN];
	int firewall;
	int portcount;
	int portlist[NMAP_PORT_COUNT];
};

struct di_fp_hash
{
    int     buckets;
    struct list_head   *table;
};

struct di_dev_fp_info
{
	int id;
	int sysclass_id;
	int portcount;
	int portlist[NMAP_PORT_COUNT];
	int devtype_id;
};

struct di_dev_fp_node
{
	struct list_head hnode;
	struct di_dev_fp_info fp_info;
};


struct di_content_fp_s
{
	unsigned int size;
	unsigned int line_num;
	unsigned int word_num;
	int systype_id;
	int devtype_id;
};

struct di_content_fp_node_s
{
	struct list_head lhash;
	di_content_fp_t info;
};

struct di_telnet_fp_s
{
	char key_text[DI_KEY_TEXT];
	int systype_id;
	int devtype_id;
};

struct di_telnet_fp_node_s
{
	struct list_head list;
	di_telnet_fp_t info;
};

struct di_devtype
{
	int devtype_id;
	int devclass_id;
	char key_name[NMAP_NAME_LEN];
	char en_name[NMAP_NAME_LEN];
	char cn_name[NMAP_NAME_LEN];
	char en_class_name[NMAP_NAME_LEN];
	char cn_class_name[NMAP_NAME_LEN];
};


#ifdef __cplusplus
}
#endif


#endif

