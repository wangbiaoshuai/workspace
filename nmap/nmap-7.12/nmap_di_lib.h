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
	UNKNOW_DEV = 0, //δ֪�豸
	ALL_DEV, //�����豸
	PC,             //���˵���
	THIN_CLIENTS, //�ݿͻ���
	VMWARE, //�����
	MOBILE,     //�ƶ��豸
	PRINTER,  //�����ӡ��
	IP_PHONE, //IP�绰
	CAMERA,    //���������
	POS,    //POS��
	SWITCH, //������
	ROUTER, //·����
	WIRELESS_ROUTER, //����·����
	WAP,      //����AP
	NETDEV, //�����豸
	AD_SERVER, //AD������
	LDAP_SERVER, //LDAP������
	WINDOWS_SERVER, //WINDOWS������
	LINUX_SERVER, //Linux������
	VIDEO_STORAGE_SERVER, //��Ƶ�洢������
	HARDDISK_VIDEO_RECORDER,//����Ӳ��¼���
	CONSOLE_SERVER,//���ڷ�����
	FAX_MACHINE,//�����
	SCANNER,//ɨ����
	ATTENDANCE_MACHINE,//���ڻ�
	ENTRANCE_GUARD,//�Ž�
	SSPI_TERMINAL,//�����ɷѲ�ѯ�ն�
	UPS,//UPS����
	CAS_SYSTEM,//CAC���߼��װ��ϵͳ
	TIME_ATTENDANCE,//����ϵͳ
	PEM_HOST,//���������������
	POLYCOM,//Polycom�ն�
	IVC_TERMINAL,//һ�廯��Ƶ�����ն�
	MCU_SERVER,//MCU������
	VCV_SERVER,//��Ƶ����¼��������
	VM_PLATFORM,//��Ƶ���ƽ̨
	DVR,//����Ӳ��¼���
	DNBZZG_SYSTEM,//���ܱ������ת��ϵͳ
	CLOUD_TERMINAL,//���ն�
	NAS_SYSTEM,//NASϵͳ
	VMWARE_VSPHERE_PLATFORM,//vSphere���⻯ƽ̨
	VMWARE_ESXI_PLATFORM,//ESXi���⻯ƽ̨
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

