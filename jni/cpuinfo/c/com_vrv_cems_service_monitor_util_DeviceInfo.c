#include "com_vrv_cems_service_monitor_util_DeviceInfo.h"
#include <string.h>
#include <stdio.h>

/*
 * Class:     com_vrv_cems_service_monitor_util_DeviceInfo
 * Method:    GetCpuInfo
 * Signature: ()Lcom/vrv/cems/service/monitor/util/CpuInfo;
 */
JNIEXPORT jobject JNICALL Java_com_vrv_cems_service_monitor_util_DeviceInfo_GetCpuInfo
  (JNIEnv *env, jobject obj)
{
    char cpu_processor[10] = {0};
    char cpu_rate[10] = {0};
    char cpu_arch[10] = {0};
    char line[512] = {0};
    int cpu_num = 0;
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if(fp)
    {
        while(!feof(fp))
        {
            memset(line, 0, sizeof(line));
            fgets(line, sizeof(line) - 1, fp);
            if(feof(fp))
                break;
            line[strlen(line) - 1] = 0;
            switch(line[0])
            {
            case 'p':
                if(strncmp(line, "processor", 9) == 0)
                {
                    cpu_num++;
                }
                break;
            case 'c':
                if(strncmp(line, "cpu MHz", 7) == 0)
                {
                    char* res = strtok(line, " ");
                    while(res)
                    {
                        strcpy(cpu_rate, res);
                        res = strtok(NULL, " ");
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    fclose(fp);

    sprintf(cpu_processor, "%d", cpu_num);

    fp = fopen("/proc/asound/oss/sndstat", "r");
    if(fp)
    {
        while(!feof(fp))
        {
            memset(line, 0, sizeof(line));
            fgets(line, sizeof(line) - 1, fp);
            if(feof(fp))
                break;
            line[strlen(line) - 1] = 0;
            switch(line[0])
            {
            case 'K':
                if(strncmp(line, "Kernel", 6) == 0)
                {
                    char* res = strtok(line, " ");
                    while(res)
                    {
                        strcpy(cpu_arch, res);
                        res = strtok(NULL, " ");
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    fclose(fp);
    //printf("cpu_processor: %s, cpu_rate: %s, cpu_arch: %s\n", cpu_processor, cpu_rate, cpu_arch);
    jclass cls = (*env)->FindClass(env, "com/vrv/cems/service/monitor/util/CpuInfo");
    jmethodID id = (*env)->GetMethodID(env, cls, "<init>", "()V");
    jobject paramout = (*env)->NewObjectA(env, cls, id, 0);

    jfieldID cpu_arch_id = (*env)->GetFieldID(env, cls, "cpu_arch", "Ljava/lang/String;");
    jfieldID cpu_rate_id = (*env)->GetFieldID(env, cls, "cpu_rate", "Ljava/lang/String;");
    jfieldID cpu_processor_id = (*env)->GetFieldID(env, cls, "cpu_processor", "Ljava/lang/String;");

    (*env)->SetObjectField(env, paramout, cpu_arch_id, (jstring)(*env)->NewStringUTF(env, cpu_arch));
    (*env)->SetObjectField(env, paramout, cpu_rate_id, (jstring)(*env)->NewStringUTF(env, cpu_rate));
    (*env)->SetObjectField(env, paramout, cpu_processor_id, (jstring)(*env)->NewStringUTF(env, cpu_processor));
    return paramout;
}

/*
 * Class:     com_vrv_cems_service_monitor_util_DeviceInfo
 * Method:    GetNetInfo
 * Signature: ()Lcom/vrv/cems/service/monitor/util/NetInfo;
 */
JNIEXPORT jobject JNICALL Java_com_vrv_cems_service_monitor_util_DeviceInfo_GetNetInfo
  (JNIEnv *env, jobject obj)
{
    char mac[20] = {0};
    char ip[20] = {0};

    FILE* fp = popen("ifconfig | egrep 'HWaddr|硬件地址' | awk '{print $5}'", "r");
    if(fp)
    {
        fgets(mac, sizeof(mac) - 1, fp);
        mac[strlen(mac) - 1] = 0;
    }
    pclose(fp);

    fp = popen("ifconfig |grep inet |grep -v 127.0.0.1 |grep -v inet6 |awk '{print $2}' |tr -d 'addr 地址:'", "r");
    if(fp)
    {
        fgets(ip, sizeof(ip) - 1, fp);
        ip[strlen(ip) - 1] = 0;
    }
    pclose(fp);

    jclass cls = (*env)->FindClass(env, "com/vrv/cems/service/monitor/util/NetInfo");
    jmethodID id = (*env)->GetMethodID(env, cls, "<init>", "()V");
    jobject paramout = (*env)->NewObjectA(env, cls, id, 0);

    jfieldID mac_id = (*env)->GetFieldID(env, cls, "mac", "Ljava/lang/String;");
    jfieldID ip_id = (*env)->GetFieldID(env, cls, "ip", "Ljava/lang/String;");
 
    (*env)->SetObjectField(env, paramout, mac_id, (jstring)(*env)->NewStringUTF(env, mac));
    (*env)->SetObjectField(env, paramout, ip_id, (jstring)(*env)->NewStringUTF(env, ip));
    return paramout;
}

/*
 * Class:     com_vrv_cems_service_monitor_util_DeviceInfo
 * Method:    GetDiskInfo
 * Signature: ()Ljava/util/ArrayList;
 */
typedef struct DiskInfo
{
    char disk_name[20];
    char disk_capacity[20];
    char disk_use_rate[20];
} DiskInfo;

JNIEXPORT jobject JNICALL Java_com_vrv_cems_service_monitor_util_DeviceInfo_GetDiskInfo
  (JNIEnv *env, jobject obj)
{
    DiskInfo array_disk_info[15] = {0};
    char line[512] = {0};
    FILE* fp = popen("df -lhk -P | egrep -v 'Filesystem|文件系统' | awk '{print $6,$2,$3}'", "r");
    int i = 0;
    if(fp)
    {
        while(fgets(line, sizeof(line) - 1, fp) != NULL)
        {
            int length = strlen(line);
            char* res = strtok(line, " ");
            if(res)
            {
                strncpy(array_disk_info[i].disk_name, res, strlen(res));
                //printf("disk_name: %s", array_disk_info[i].disk_name);
            }
            
            if((res = strtok(NULL, " ")) != NULL)
            {
                strncpy(array_disk_info[i].disk_capacity, res, strlen(res));
                //printf(", disk_capacity: %s", array_disk_info[i].disk_capacity);
            }

            int offset = strlen(array_disk_info[i].disk_name) + strlen(array_disk_info[i].disk_capacity) + 1 + 1;
            strncpy(array_disk_info[i].disk_use_rate, line + offset, length - offset);
            //printf(", disk_use_rate: %s\n", array_disk_info[i].disk_use_rate);
            i++;
        }
    }
    pclose(fp);

    jclass cls = (*env)->FindClass(env, "java/util/ArrayList");
    jmethodID id = (*env)->GetMethodID(env, cls, "<init>", "()V");
    jobject vec_obj = (*env)->NewObjectA(env, cls, id, 0); 
    jmethodID vec_put_method_id = (*env)->GetMethodID(env, cls, "add", "(Ljava/lang/Object;)Z");

    for(int j = 0; j < i; j++)
    {
        jclass disk_cls = (*env)->FindClass(env, "com/vrv/cems/service/monitor/util/DiskInfo");
        jmethodID disk_id = (*env)->GetMethodID(env, disk_cls, "<init>", "()V");
        jobject disk_obj = (*env)->NewObjectA(env, disk_cls, disk_id, 0);

        jfieldID disk_name_id = (*env)->GetFieldID(env, disk_cls, "disk_name", "Ljava/lang/String;");
        jfieldID disk_capacity_id = (*env)->GetFieldID(env, disk_cls, "disk_capacity", "Ljava/lang/String;");
        jfieldID disk_use_rate_id = (*env)->GetFieldID(env, disk_cls, "disk_use_rate", "Ljava/lang/String;");
        (*env)->SetObjectField(env, disk_obj, disk_name_id, (jstring)(*env)->NewStringUTF(env, array_disk_info[j].disk_name));
        (*env)->SetObjectField(env, disk_obj, disk_capacity_id, (jstring)(*env)->NewStringUTF(env, array_disk_info[j].disk_capacity));
        (*env)->SetObjectField(env, disk_obj, disk_use_rate_id, (jstring)(*env)->NewStringUTF(env, array_disk_info[j].disk_use_rate));

        (*env)->CallObjectMethod(env, vec_obj, vec_put_method_id, disk_obj);

    }
    return vec_obj;
}

/*
 * Class:     com_vrv_cems_service_monitor_util_DeviceInfo
 * Method:    GetSysInfo
 * Signature: ()Lcom/vrv/cems/service/monitor/util/SysInfo;
 */
JNIEXPORT jobject JNICALL Java_com_vrv_cems_service_monitor_util_DeviceInfo_GetSysInfo
  (JNIEnv *env, jobject obj)
{
    char* sys_type = "Linux";
    char sys_version[30] = {0};
    char hostname[20] = {0};

    FILE* fp = fopen("/etc/issue", "r");
    if(fp)
    {
        fgets(sys_version, sizeof(sys_version) - 1, fp);
        sys_version[strlen(sys_version) - 1] = 0;
    }
    fclose(fp);

    FILE* fp_p = popen("hostname", "r");
    if(fp_p)
    {
        fgets(hostname, sizeof(hostname) - 1, fp_p);
    }
    pclose(fp_p);

    jclass sys_cls = (*env)->FindClass(env, "com/vrv/cems/service/monitor/util/SysInfo");
    jmethodID sys_id = (*env)->GetMethodID(env, sys_cls, "<init>", "()V");
    jobject sys_obj = (*env)->NewObjectA(env, sys_cls, sys_id, 0);

    jfieldID sys_type_id = (*env)->GetFieldID(env, sys_cls, "system_type", "Ljava/lang/String;");
    jfieldID sys_version_id = (*env)->GetFieldID(env, sys_cls, "system_version", "Ljava/lang/String;");
    jfieldID hostname_id = (*env)->GetFieldID(env, sys_cls, "hostname", "Ljava/lang/String;");
    (*env)->SetObjectField(env, sys_obj, sys_type_id, (jstring)(*env)->NewStringUTF(env, sys_type));
    (*env)->SetObjectField(env, sys_obj, sys_version_id, (jstring)(*env)->NewStringUTF(env, sys_version));
    (*env)->SetObjectField(env, sys_obj, hostname_id, (jstring)(*env)->NewStringUTF(env, hostname));

    return sys_obj;
}
