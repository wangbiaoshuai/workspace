/** 
   * @file statfs.c
   * @brief get disk infomation
   * @author T-bagwell@ChinaUnix.net
   * @date 2009-12-12
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>

struct disk_infomation{
    char disk_format[8];
    char disk_total_capacity[11];
    char disk_free_capacity[11];
    char disk_permissions[3];
};

static int get_disk_infomation(const char *path, struct disk_infomation *di);
static char *convert_size(float size, char *dest);
static unsigned long kscale(unsigned long b, unsigned long bs) ;

int main(int argc, char *argv[])
{
    struct disk_infomation *di=malloc(sizeof(struct disk_infomation));
    char *path =(char *)malloc(20);

    if(argc < 2)
    {
        printf("%s Mount point of the dir\n", argv[0]);
        return 0;
    }
    strncpy(path, argv[1], strlen(argv[1]));
    get_disk_infomation(path, di);

    printf("DiskType:            %s\n", di->disk_format);
    printf("DiskTotalCapacity    %s\n", di->disk_total_capacity);
    printf("DiskFreeCapacity    %s\n", di->disk_free_capacity);
    printf("DiskPermissions        %s\n", di->disk_permissions);

    free(di);
    free(path);
    return 0;
}

/** 
   * @brief block to byte 
   * @param b     block
   * @param bs     KByte
   * @return         Kbyte
   */
static unsigned long kscale(unsigned long b, unsigned long bs) 
{
    return (b * (unsigned long long) bs + 1024/2) / 1024;
}

/** 
   * @brief get disk informations 
   * @param path     the disk mount point
   * @param di     the structure of the disk information
   * @return         success: 1, faild: 0
   */
static int get_disk_infomation(const char *path, struct disk_infomation *di)
{
    struct statfs buf;
    int i=0;
    float disk_total_size = 0;
    float disk_free_size = 0;

    i=statfs(path, &buf);
    if(i < 0)
    {
        printf("get disk infomation faild\n");
        return 0;
    }
    switch(buf.f_type)
    {
        memset(di->disk_format, 0, 8);
    case 0x4d44:
        sprintf(di->disk_format, "FAT");
        break;

    case 0x5346544e:
    case 0X65735546: 
        sprintf(di->disk_format, "NTFS");
        break;

    case 0xEF53:
    case 0xEF51:
        sprintf(di->disk_format, "EXT2");
        break;

    default:
        sprintf(di->disk_format, "unknown");
        break;
    }


    memset(di->disk_total_capacity, 0, 10);
    memset(di->disk_free_capacity, 0, 10);

    disk_total_size = (float)(kscale(buf.f_blocks, buf.f_bsize));
    disk_free_size = (float)(kscale(buf.f_bfree, buf.f_bsize));

    convert_size(disk_total_size, di->disk_total_capacity);
    convert_size(disk_free_size, di->disk_free_capacity);

    memset(di->disk_permissions, 0, 3);
    sprintf(di->disk_permissions, "rw\n");
    return 1;
}


/** 
   * @name convert_size
   * @brief convert size to GB MB KB
   *         size:    the size
   *         dest:    the return size like string
   * @author JerryLeo
   * @date 2009-12-12
   */
static char *convert_size(float size, char *dest)
{
    if((((size/1024.0)/1024.0)) >= 1.0)
    { 
        sprintf(dest, "%0.2fGB", ((size/1024.0)/1024.0));
    } 
    else if(((size/1024.0)) >= 1.0)
    { 
        sprintf(dest, "%0.2fMB", (size/1024.0));
    } 
    else
    {
        sprintf(dest, "%0.2fKB", size);
    } 
    return dest;
}
