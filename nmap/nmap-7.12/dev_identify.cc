/***************************************************************************
 * nmap_identify.cc -- the interface implementation of nmap scan for dev identify.  
 ***********************IMPORTANT NMAP LICENSE TERMS************************/


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "nmap.h"
#include "utils.h"
#include "logsys.h"
#include "nmap_di_lib.h"
#include "cJSON.h"
#include "jhash.h"

struct di_fp_hash dev_fp_hash; //设备指纹hash表
struct list_head di_content_htable[DI_CONTENT_HASH_SIZE];
struct list_head di_telnet_list;
struct di_devtype devtype_info[DEV_TYPE_MAX];
int devtype_count = 0;

char *di_sysclass_name[SYSCLASS_NUM] = {
	"unknow",
	"all",
	"Windows",
	"Windows Mobile",
	"Windows Server",
	"Linux",
	"Android",
	"Cisco IOS",
	"Apple iOS",
	"Mac OS X",
	"VxWorks",
	"embedded",
	"Huawei VRP",
	"H3C Comware",
	"VMware ESXi",
	"FreeBSD",
	"APC AOS",
};

int di_sysclass_get_name_by_systype_name(char *systype_name, char *sysclass_name, int size)
{
	int count = 0;
	int retval = 0;
	int len = 0;
	int i = 0;

	count = sizeof(di_sysclass_name)/sizeof(char *);
	
	for (i=0; i<count; i++)
	{
		if (strstr(systype_name,di_sysclass_name[i]) != NULL)
		{
			len = size<strlen(di_sysclass_name[i])?size:strlen(di_sysclass_name[i]);			
			strncpy(sysclass_name,di_sysclass_name[i],len);
			sysclass_name[len] = '\0';
			retval = 1;
			break;
		}
	}

	return retval;
}

int di_get_sysclass_id_by_name(char *sysclass_name)
{
	int count = 0;
	int i = 0;

	if (strlen(sysclass_name) == 0)
	{
		return 0;
	}

	count = sizeof(di_sysclass_name)/sizeof(char *);
	
	for (i=0; i<count; i++)
	{
		if (strcmp(sysclass_name,di_sysclass_name[i]) == 0)
		{
			return i;
		}
	}

	return 0;
}

int di_get_devtype_id_by_name(char *devtype_name)
{
	int i = 0;

	if (strlen(devtype_name) == 0)
	{
		return 0;
	}
	
	for (i=0; i<devtype_count; i++)
	{
		if (strcmp(devtype_name,devtype_info[i].key_name) == 0)
		{
			return i;
		}
	}

	return 0;
}


unsigned int vht_mkhash_number(unsigned int number, unsigned int mask)
{
    return jhash_1word(number, 0) % mask;
}

#ifndef DI_TELNET_FP_MATCH
di_telnet_fp_node_t *di_telnet_malloc_node(void)
{
	di_telnet_fp_node_t *node = NULL;

	node = (di_telnet_fp_node_t *)malloc(sizeof(di_telnet_fp_node_t));
	if (node == NULL)
	{
		return NULL;
	}

	memset(node,0,sizeof(di_telnet_fp_node_t));
	INIT_LIST_HEAD(&node->list);

	return node;
}

di_telnet_fp_node_t *di_telnet_match_find_node(char *telnet_text)
{
	di_telnet_fp_node_t *node = NULL;
	
	//pthread_rwlock_rdlock(&di_telnet_rwlock);
    list_for_each_entry(node, &di_telnet_list, list)
    {
		if (strstr(telnet_text,node->info.key_text) != NULL)
		{
			//pthread_rwlock_unlock(&di_telnet_rwlock);
			return node;
		}
    }
   // pthread_rwlock_unlock(&di_telnet_rwlock);

	return NULL;
}


di_telnet_fp_node_t *di_telnet_find_node(char *telnet_text)
{
	di_telnet_fp_node_t *node = NULL;
	
	//pthread_rwlock_rdlock(&di_telnet_rwlock);
    list_for_each_entry(node, &di_telnet_list, list)
    {
		if (strcmp(telnet_text,node->info.key_text) == 0)
		{
			//pthread_rwlock_unlock(&di_telnet_rwlock);
			return node;
		}
    }
    //pthread_rwlock_unlock(&di_telnet_rwlock);

	return NULL;
}

int di_get_telnet_info(unsigned int ipaddr, int port,char *telnet_text, int size)
{
	FILE *fp = NULL;
	char line[512];
	char filename[64];
	char cmd[64];
	int len = 0;

	memset(cmd,0,sizeof(cmd));
	if (port == 23)
	{
		snprintf(cmd,64,"python /usr/local/devscan/devscan_telnet.py "NMAP_FMT_IP"",NMAP_STR_IP(ipaddr));
	}
	else
	{
		return -1;
	}

	system(cmd);

	memset(filename,0,sizeof(filename));
	snprintf(filename,64,"/tmp/content/telnet_"NMAP_FMT_IP"",NMAP_STR_IP(ipaddr));
	
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		log_debug("open file %s",filename);
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		len += snprintf(telnet_text+len,size-len,"%s",line);
	}

	fclose(fp);

	return 0;
}

int di_telnet_fp_add_deal(di_telnet_fp_t *info)
{
	di_telnet_fp_node_t *node = NULL;

	node = di_telnet_find_node(info->key_text);
	if (node == NULL)
	{
		node = di_telnet_malloc_node();
		if (node == NULL)
		{
			return -1;
		}

		memcpy(&node->info,info,sizeof(di_telnet_fp_t));

		//pthread_rwlock_wrlock(&di_telnet_rwlock);
		list_add(&node->list, &di_telnet_list);
		//pthread_rwlock_unlock(&di_telnet_rwlock);

		return 0;
	}
	
	return 1;
}

int di_telnet_fp_match(unsigned int ipaddr, unsigned short port,int *devtype_id,int *systype_id)
{
	di_telnet_fp_node_t *node = NULL;
	di_telnet_fp_t info;
	char telnet_text[2048];
	int retval = 0;

	memset(&info,0,sizeof(info));
	memset(telnet_text,0,sizeof(telnet_text));
	retval = di_get_telnet_info(ipaddr,port,telnet_text,2048);
	if (retval != 0)
	{
		return 0;
	}
	
	node = di_telnet_match_find_node(telnet_text);
	if (node == NULL)
	{
		return 0;
	}

	log_debug("find telnet text.");

	*devtype_id = node->info.devtype_id;
	*systype_id = node->info.systype_id;

	return 1;
}

void di_telnet_fp_destory(void)
{
	di_telnet_fp_node_t *fp_node = NULL;
	di_telnet_fp_node_t *tmp_node = NULL;

	//pthread_rwlock_rdlock(&di_telnet_rwlock);
	list_for_each_entry_safe(fp_node, tmp_node,&di_telnet_list, list)
	{
	   list_del(&fp_node->list);
	   free(fp_node);
	   fp_node = NULL;
	}
	//pthread_rwlock_unlock(&di_telnet_rwlock);
}

int di_telnet_fp_init(void)
{
	FILE *fp = NULL;
	di_telnet_fp_t telnet_fp;
	cJSON *root = NULL;
	char line[256];

	INIT_LIST_HEAD(&di_telnet_list);

	fp = fopen(DI_TELNET_FP_FILE, "r");
	if (fp == NULL) 
	{
		log_debug("open file %s",DI_TELNET_FP_FILE);
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != NULL)
	{	
		memset(&telnet_fp,0,sizeof(telnet_fp));

		root = cJSON_Parse(line);
		if (root == NULL)
		{
			log_error("%s_%d,cJSON_Parse() fail",__FUNCTION__,__LINE__);
			return -1;
		}

		/*parse key text*/
		cJSON *json_key_text = cJSON_GetObjectItem(root, "key_text");
		if (json_key_text != NULL && strlen(json_key_text->valuestring) != 0)
		{
			snprintf(telnet_fp.key_text,DI_KEY_TEXT-1,"%s",json_key_text->valuestring);
		}

		cJSON *json_systype_id = cJSON_GetObjectItem(root, "systype_id");
		if (json_systype_id != NULL)
		{
			telnet_fp.systype_id = json_systype_id->valueint;
		}

		cJSON *json_devtype_id = cJSON_GetObjectItem(root, "devtype_id");
		if (json_devtype_id != NULL)
		{
			telnet_fp.devtype_id = json_devtype_id->valueint;
		}
		
		di_telnet_fp_add_deal(&telnet_fp);
	}

	fclose(fp);
		
	return 0;
}
#endif

#ifndef DI_CONTENT_FP_MATCH
di_content_fp_node_t *di_content_malloc_node(void)
{
	di_content_fp_node_t *node = NULL;

	node = (di_content_fp_node_t *)malloc(sizeof(di_content_fp_node_t));
	if (node == NULL)
	{
		return NULL;
	}

	memset(node,0,sizeof(di_content_fp_node_t));
	INIT_LIST_HEAD(&node->lhash);

	return node;
}

di_content_fp_node_t *di_content_find_node(di_content_fp_t *info)
{
	di_content_fp_node_t *node = NULL;
	unsigned int hash = 0;

	hash = vht_mkhash_number(info->size, DI_CONTENT_HASH_SIZE);
	
	//pthread_rwlock_rdlock(&di_content_rwlock[hash]);
    list_for_each_entry(node, &di_content_htable[hash], lhash)
    {
        if ((node->info.size== info->size) && (node->info.line_num == info->line_num) && (node->info.word_num == info->word_num))
        {
            //pthread_rwlock_unlock(&di_content_rwlock[hash]);
            return node;
        }
    }
    //pthread_rwlock_unlock(&di_content_rwlock[hash]);

	return NULL;
}

int di_get_content_info(unsigned int ipaddr, int port,di_content_fp_t *info)
{
	char cmd[64] = {0};
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	int count = 0;
	char *endptr = NULL;

	if (port == 80)
	{
		snprintf(cmd,64,"/usr/local/devscan/devscan_wget.sh "NMAP_FMT_IP"",NMAP_STR_IP(ipaddr));
	}
	else
	{
		return -1;
	}
	
	fp = popen(cmd, "r");
    if (fp == NULL)
    {
        log_error("%s_%d,popen()",__FUNCTION__,__LINE__);
        return -1;
    }

    while (getline(&line, &len, fp) != -1)
    {
    	if (count == 0)
    	{
			info->line_num = strtoul(line, &endptr, 10);
		}
		else if (count == 1)
    	{
			info->word_num = strtoul(line, &endptr, 10);
		}
		else if (count == 2)
    	{
			info->size = strtoul(line, &endptr, 10);
		}

		count++;
    }

    if (count == 3)
    {
		pclose(fp);
		return 0;
    }

    pclose(fp);

	return -1;
}


int di_content_fp_add_deal(di_content_fp_t *info)
{
	di_content_fp_node_t *node = NULL;
	unsigned int hash = 0;

	node = di_content_find_node(info);
	if (node == NULL)
	{
		node = di_content_malloc_node();
		if (node == NULL)
		{
			return -1;
		}

		memcpy(&node->info,info,sizeof(di_content_fp_t));

		hash = vht_mkhash_number(info->size, DI_CONTENT_HASH_SIZE);
		//pthread_rwlock_wrlock(&di_content_rwlock[hash]);
		list_add(&node->lhash, &di_content_htable[hash]);
		//pthread_rwlock_unlock(&di_content_rwlock[hash]);

		return 0;
	}
	
	return 1;
}

int di_content_fp_match(unsigned int ipaddr, unsigned short port,int *devtype_id,int *systype_id)
{
	di_content_fp_node_t *node = NULL;
	di_content_fp_t info;
	int retval = 0;

	memset(&info,0,sizeof(info));
	retval = di_get_content_info(ipaddr,port,&info);
	if (retval != 0)
	{
		return 0;
	}
	
	node = di_content_find_node(&info);
	if (node == NULL)
	{
		return 0;
	}

	*devtype_id = node->info.devtype_id;
	*systype_id = node->info.systype_id;

	return 1;
}

void di_content_fp_destory(void)
{
	di_content_fp_node_t *fp_node = NULL;
	di_content_fp_node_t *tmp_node = NULL;
	int i = 0;

	for (i=0; i<DI_CONTENT_HASH_SIZE; i++)
	{
		//pthread_rwlock_rdlock(&di_content_rwlock[i]);
		list_for_each_entry_safe(fp_node, tmp_node,&di_content_htable[i], lhash)
		{
		   list_del(&fp_node->lhash);
		   free(fp_node);
		   fp_node = NULL;
		}
		//pthread_rwlock_unlock(&di_content_rwlock[i]);
	}

	log_info("dev content destory success.");
}

int di_content_fp_init(void)
{
	FILE *fp = NULL;
	di_content_fp_t content_fp;
	cJSON *root = NULL;
	char line[256];
	int hash = 0;

	for (hash=0; hash<DI_CONTENT_HASH_SIZE; hash++)
	{
		INIT_LIST_HEAD(&di_content_htable[hash]);
	}

	fp = fopen(DI_CONTENT_FP_FILE, "r");
	if (fp == NULL) 
	{
		printf("open file %s",DI_CONTENT_FP_FILE);
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != NULL)
	{	
		memset(&content_fp,0,sizeof(content_fp));

		root = cJSON_Parse(line);
		if (root == NULL)
		{
			log_error("%s_%d,cJSON_Parse() fail",__FUNCTION__,__LINE__);
			return -1;
		}

		cJSON *json_size = cJSON_GetObjectItem(root, "size");
		if (json_size != NULL)
		{
			content_fp.size = json_size->valueint;
		}
		
		cJSON *json_line = cJSON_GetObjectItem(root, "line");
		if (json_line != NULL)
		{
			content_fp.line_num = json_line->valueint;
		}

		cJSON *json_word = cJSON_GetObjectItem(root, "word");
		if (json_word != NULL)
		{
			content_fp.word_num = json_word->valueint;
		}

		cJSON *json_systype_id = cJSON_GetObjectItem(root, "systype_id");
		if (json_systype_id != NULL)
		{
			content_fp.systype_id = json_systype_id->valueint;
		}

		cJSON *json_devtype_id = cJSON_GetObjectItem(root, "devtype_id");
		if (json_devtype_id != NULL)
		{
			content_fp.devtype_id = json_devtype_id->valueint;
		}
		
		di_content_fp_add_deal(&content_fp);
	}

	fclose(fp);
		
	return 0;
}
#endif

#ifndef DI_SYS_FP_MNG
int di_dev_fp_node_init(struct di_dev_fp_info *fp_info)
{
	struct di_dev_fp_node *fp_node = NULL;
	int hash = fp_info->sysclass_id;

	if (hash < 0 || hash >= dev_fp_hash.buckets)
	{
		log_debug("The sysclass id[%d] is error",hash);
		return -1;
	}

	fp_node = (struct di_dev_fp_node *)malloc(sizeof(struct di_dev_fp_node));
	if (fp_node == NULL)
	{
		log_error("malloc dev fingerprint node fail");
		return -1;
	}

	/*初始化节点*/
	INIT_LIST_HEAD(&fp_node->hnode);
	memcpy(&fp_node->fp_info, fp_info,sizeof(struct di_dev_fp_info));

	/*添加到hash表中*/
	//pthread_rwlock_wrlock(&dev_fp_hash.rwlock);
    list_add_tail(&fp_node->hnode, &dev_fp_hash.table[hash]);
    //pthread_rwlock_unlock(&dev_fp_hash.rwlock);
	
	return 0;
}

int di_dev_fp_match(struct di_dev_fp_info *fp_info, struct devscan_result_s *nmap_result)
{
	int i,j;
	int port_match_num = 0; //匹配次数

	/*匹配端口*/
	if (fp_info->portcount!= 0)
	{
		for (i=0; i<fp_info->portcount; i++)
		{
			for (j=0; j<nmap_result->portcount; j++)
			{
				if (fp_info->portlist[i] == nmap_result->portlist[j])
				{
					port_match_num++;
					break;
				}
			}	
		}

		if (port_match_num != fp_info->portcount)
		{
			return 0;
		}
	}

	return 1;
}

int di_dev_fp_lib_match(struct devscan_result_s *nmap_result)
{
	struct di_dev_fp_node *fp_node = NULL;
	int hash = nmap_result->sysclass_id;

	if (hash >= dev_fp_hash.buckets)
	{
		log_debug("fp lib match:the sysclass id[%d] is error",hash);
		return -1;
	}
	

    //pthread_rwlock_rdlock(&dev_fp_hash.rwlock);
    list_for_each_entry(fp_node, &dev_fp_hash.table[hash], hnode)
    {
        if (di_dev_fp_match(&fp_node->fp_info,nmap_result) == 1)	
		{
			nmap_result->devtype_id = fp_node->fp_info.devtype_id;
			//pthread_rwlock_unlock(&dev_fp_hash.rwlock);
			return 0;
		}
    }
    //pthread_rwlock_unlock(&dev_fp_hash.rwlock);

	return -1;
}


int di_dev_fp_apple_match(struct devscan_result_s *nmap_result)
{
	int i = 0;

	for (i=0; i<nmap_result->portcount; i++)
	{
		/*苹果手机特殊处理*/
		if (nmap_result->portlist[i] == 62078) //62078/tcp open iphone-sync
		{
			nmap_result->devtype_id = MOBILE;
			nmap_result->sysclass_id = APPLE_IOS;
			snprintf(nmap_result->systype_name,NMAP_NAME_LEN-1,"%s","Apple iOS");
			snprintf(nmap_result->sysclass_name,NMAP_NAME_LEN-1,"%s","Apple iOS");
	
			return 0;
		}
	}

	if (nmap_result->sysclass_id == 0)
	{
		return -1;
	}
	
	if (nmap_result->sysclass_id == MAC_OS_X && nmap_result->devtype_id == UNKNOW_DEV)
	{
		nmap_result->devtype_id = PC;
		nmap_result->sysclass_id = MAC_OS_X;
		snprintf(nmap_result->systype_name,NMAP_NAME_LEN-1,"%s","Mac OS X");
		snprintf(nmap_result->sysclass_name,NMAP_NAME_LEN-1,"%s","Mac OS X");

		return 0;
	}

	return -1;
}


int di_fp_match_port(unsigned int ipaddr,struct devscan_result_s *nmap_result)
{
	int i = 0;
	int devtype_id = 0;
	int systype_id = 0;
	int retval = 0;
	
	for (i=0; i<nmap_result->portcount; i++)
	{
		if (nmap_result->portlist[i] == 80)
		{
			retval = di_content_fp_match(ipaddr,80,&devtype_id,&systype_id);
			if (retval == 1)
			{
				nmap_result->devtype_id = devtype_id;
				if (nmap_result->sysclass_id != 0)
				{
					nmap_result->sysclass_id = systype_id;
				}
			}
			return 0;
		}
		else if (nmap_result->portlist[i] == 23)
		{
			retval = di_telnet_fp_match(ipaddr,23,&devtype_id,&systype_id);
			if (retval == 1)
			{
				nmap_result->devtype_id = devtype_id;
				if (nmap_result->sysclass_id != 0)
				{
					nmap_result->sysclass_id = systype_id;
				}
			}
			return 0;
		}
	}

	return 0;
}

void di_dev_fp_destory(void)
{
	struct di_dev_fp_node *fp_node = NULL;
	struct di_dev_fp_node *tmp_node = NULL;
	int i = 0;

    //pthread_rwlock_wrlock(&dev_fp_hash.rwlock);
    for (i=0; i<SYSCLASS_NUM; i++)
    {
	    list_for_each_entry_safe(fp_node, tmp_node,&dev_fp_hash.table[i], hnode)
	    {
	       list_del(&fp_node->hnode);
	       free(fp_node);
	       fp_node = NULL;
	    }
    }
    //pthread_rwlock_unlock(&dev_fp_hash.rwlock);

    log_info("dev fingerprint destory success.");
}

int di_dev_fp_init(void)
{
	FILE *fp = NULL;
	struct di_dev_fp_info fp_info;
	char portlist[NMAP_PORT_COUNT];
	char line[256];
	char *str = NULL;
	char *p = NULL;
	cJSON *root = NULL;
	int fp_id = 1;
	int i = 0;

    fp = fopen(DI_DEV_FP_FILE, "r");
	if (fp == NULL) 
	{
		log_debug("open file %s",DI_DEV_FP_FILE);
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != NULL)
	{	
		memset(&fp_info,0,sizeof(fp_info));

		root = cJSON_Parse(line);
		if (root == NULL)
		{
			log_error("%s_%d,cJSON_Parse() fail",__FUNCTION__,__LINE__);
			return -1;
		}

		/*parse systype name*/
		cJSON *json_systype_name = cJSON_GetObjectItem(root, "systype_name");
		if (json_systype_name != NULL && strlen(json_systype_name->valuestring) != 0)
		{
			fp_info.sysclass_id = di_get_sysclass_id_by_name(json_systype_name->valuestring);
		    if (fp_info.sysclass_id == -1)
		    {
				log_error("%s_%d,don't find sysclass id by sysclass name[%s]",__FUNCTION__,__LINE__,json_systype_name->valuestring);
				continue;
		    }
		}

		/*parse port list*/
		cJSON *json_portlist = cJSON_GetObjectItem(root, "portlist");
		if (json_portlist != NULL && strlen(json_portlist->valuestring) != 0)
		{
			snprintf(portlist,NMAP_PORT_COUNT-1,"%s",json_portlist->valuestring);
			str = portlist;
			i = 0;
			while((p=strchr(str,',')) != NULL)
			{
				*p = '\0';
				fp_info.portlist[i++] = atoi(str);
				str = p+1;
			}
			fp_info.portlist[i++] = atoi(str);
			fp_info.portcount = i;
		}

		/*parse devtype name*/
		cJSON *json_devtype_name = cJSON_GetObjectItem(root, "devtype_name");
		if (json_devtype_name != NULL && strlen(json_devtype_name->valuestring) != 0)
		{
			fp_info.devtype_id = di_get_devtype_id_by_name(json_devtype_name->valuestring);
		}
		
		fp_info.id = fp_id;
		if (di_dev_fp_node_init(&fp_info) == -1)
		{
			return -1;
		}

		fp_id++;
	}

	fclose(fp);

	log_info("dev fingerprint init finish.");
    
	return 0;
}

int di_dev_fp_hash_init(void)
{   
	int i = 0;
	
    memset(&dev_fp_hash, 0, sizeof(struct di_fp_hash));

    dev_fp_hash.buckets = SYSCLASS_NUM;
    dev_fp_hash.table = (struct list_head *)malloc(dev_fp_hash.buckets*sizeof(struct list_head));
    if (dev_fp_hash.table == NULL)
    {
    	printf("nmap dev fp hash table malloc fail.");
        return -1;
	}

	
	for (i=0; i< dev_fp_hash.buckets; ++i)
	{
		INIT_LIST_HEAD(&dev_fp_hash.table[i]);
	}

    return 0;
}
#endif

int di_devtype_init(void)
{
	FILE *fp = NULL;
	char line[FILE_LINE_LEN];
	char lang_class_name[NMAP_NAME_LEN] = {0};
	cJSON *root = NULL;
	int id = 0;

	fp = fopen(DI_DEVTYPE_FILE,"r");
	if (fp == NULL)
	{
		log_error("open file %s fail.",DI_DEVTYPE_FILE);
		return -1;
	}

	memset(&devtype_info,0,sizeof(devtype_info));
	while (fgets(line, sizeof(line), fp) != NULL)
	{	
		root = cJSON_Parse(line);
		if (root == NULL)
		{
			log_error("%s_%d,cJSON_Parse() fail,line=%s",__FUNCTION__,__LINE__,line);
			return -1;
		}

		/*parse name*/
		cJSON *json_name = cJSON_GetObjectItem(root, "name");
		if (json_name != NULL && strlen(json_name->valuestring) > 0)
		{
			snprintf(devtype_info[id].key_name,NMAP_NAME_LEN-1,json_name->valuestring);
		}
		
		/*parse en_name*/
		cJSON *json_lang_en_name = cJSON_GetObjectItem(root, "en_name");
		if (json_lang_en_name != NULL && strlen(json_lang_en_name->valuestring) > 0)
		{
			snprintf(devtype_info[id].en_name,NMAP_NAME_LEN-1,json_lang_en_name->valuestring);
		}

		/*parse cn_name*/
		cJSON *json_lang_name = cJSON_GetObjectItem(root, "cn_name");
		if (json_lang_name != NULL && strlen(json_lang_name->valuestring) > 0)
		{
			snprintf(devtype_info[id].cn_name,NMAP_NAME_LEN-1,json_lang_name->valuestring);
		}

		/*parse en_class_name*/
		cJSON *json_lang_en_class_name = cJSON_GetObjectItem(root, "en_class_name");
		if (json_lang_en_class_name != NULL && strlen(json_lang_en_class_name->valuestring) > 0)
		{
			snprintf(devtype_info[id].en_class_name,NMAP_NAME_LEN-1,json_lang_en_class_name->valuestring);
		}

		/*parse cn_class_name*/
		cJSON *json_lang_cn_name = cJSON_GetObjectItem(root, "cn_class_name");
		if (json_lang_cn_name != NULL && strlen(json_lang_cn_name->valuestring) > 0)
		{
			snprintf(devtype_info[id].cn_class_name,NMAP_NAME_LEN-1,json_lang_cn_name->valuestring);
		}

		/*parse class id*/
		cJSON *json_lang_class_id = cJSON_GetObjectItem(root, "class_id");
		devtype_info[id].devclass_id = json_lang_class_id->valueint;
		
		
		devtype_info[id].devtype_id = id;
		
		id++;
	}

	devtype_count = id;

	fclose(fp);

	return 0;
}


int nmap_di_lib_init(void)
{
	di_devtype_init();
	di_dev_fp_hash_init();
	di_dev_fp_init();
	di_content_fp_init();
	di_telnet_fp_init();
	
	return 0;
}

int nmap_di_lib_match(unsigned int ipaddr,struct nmap_identify_s *nmap_identify,struct devscan_result_s *nmap_result)
{
	int retval = 0;

	nmap_result->status = nmap_identify->status;
	if (nmap_result->status == 0)
	{
		return 0;
	}

	if (nmap_identify->portcount == 0 || nmap_identify->portcount > NMAP_PORT_COUNT)
	{
		return 0;
	}
	
	if (nmap_identify->systype_flag == 1)
	{
		di_sysclass_get_name_by_systype_name(nmap_identify->systype_name,nmap_identify->sysclass_name,NMAP_NAME_LEN);
	}
#if 0	
	else if (nmap_identify->os_cpe == 1)
	{
		if (strstr(nmap_identify->os_cpe_name,"windows") != NULL)
		{
			memset(nmap_identify->systype_name,0,sizeof(nmap_identify->systype_name));
			memset(nmap_identify->sysclass_name,0,sizeof(nmap_identify->sysclass_name));
			
			snprintf(nmap_identify->systype_name,NMAP_NAME_LEN-1,"%s",nmap_identify->os_cpe_name);
			snprintf(nmap_identify->sysclass_name,NMAP_NAME_LEN-1,"%s",nmap_identify->os_cpe_name);

			//log_debug("the dev["FMT_IP"] be set windows by os cpe[%s]",STR_IP(ipaddr),nmap_identify->os_cpe_name);
		}
	}
#endif

	nmap_result->portcount = nmap_identify->portcount;
	memcpy(nmap_result->portlist,nmap_identify->portlist,nmap_result->portcount*sizeof(int));
	snprintf(nmap_result->systype_name,NMAP_NAME_LEN-1,"%s",nmap_identify->systype_name);
	snprintf(nmap_result->sysclass_name,NMAP_NAME_LEN-1,"%s",nmap_identify->sysclass_name);
	nmap_result->sysclass_id = di_get_sysclass_id_by_name(nmap_result->sysclass_name);

	di_dev_fp_lib_match(nmap_result);
	di_dev_fp_apple_match(nmap_result);
	di_fp_match_port(ipaddr,nmap_result);

	if (nmap_result->devtype_id >= 0 && nmap_result->devtype_id < devtype_count)
	{
		snprintf(nmap_result->devtype_name,NMAP_NAME_LEN-1,"%s",devtype_info[nmap_result->devtype_id].cn_name);
		snprintf(nmap_result->devclass_name,NMAP_NAME_LEN-1,"%s",devtype_info[nmap_result->devtype_id].cn_class_name);
	}
	
	return 0;
}


/*
功能: 对设备进行指纹识别
输入参数:
返回值:
ipaddr: 设备ip 地址
nmap_identify:保存设备的识别信息
0:设备不存在
1:成功
-1:失败
*/
int devscan_identify(unsigned int ipaddr,struct devscan_result_s *devscan_result)
{
	int myargc;
	char **myargv = NULL;
	char command[2048];
	struct nmap_identify_s nmap_identify;

	memset(&nmap_identify,0,sizeof(nmap_identify));
	sprintf(command,"nmap -O "NMAP_FMT_IP"",NMAP_STR_IP(ipaddr));
	myargc = arg_parse(command, &myargv);
    if (myargc < 1) 
    {
      printf("NMAP_ARGS variable could not be parsed\n");
      return -1;
    }

    nmap_di_lib_init();
    nmap_identify_main(myargc, myargv,&nmap_identify);
    nmap_di_lib_match(ipaddr,&nmap_identify,devscan_result);

	return 0;
}
