package com.vrv.cems.service.monitor.util;
import java.util.ArrayList;

class CpuInfo
{
    public String cpu_arch;
    public String cpu_rate;
    public String cpu_processor;
};

class NetInfo
{
    public String mac;
    public String ip;
};

class DiskInfo
{
    public String disk_name;
    public String disk_capacity;
    public String disk_use_rate;
};

class SysInfo
{
    public String system_type;
    public String system_version;
    public String hostname;
};

class DeviceInfo
{
    static {
        System.out.println( System.getProperty("java.library.path"));
        System.loadLibrary("device_info");
    }
    public native CpuInfo GetCpuInfo();
    public native NetInfo GetNetInfo();
    public native ArrayList<DiskInfo> GetDiskInfo();
    public native SysInfo GetSysInfo();

// test
    public static void main(String[] args)
    {
        DeviceInfo device_info = new DeviceInfo();
        CpuInfo cpu_info = new CpuInfo();
        cpu_info = device_info.GetCpuInfo();
        System.out.println("cpu_arch="+cpu_info.cpu_arch+", cpu_rate="+cpu_info.cpu_rate+", cpu_processor="+cpu_info.cpu_processor);

        NetInfo net_info = new NetInfo();
        net_info = device_info.GetNetInfo();
        System.out.println("mac="+net_info.mac+", ip="+net_info.ip);

        ArrayList<DiskInfo> vec_disk_info = new ArrayList<DiskInfo>();
        vec_disk_info = device_info.GetDiskInfo();
        for(int i = 0; i < vec_disk_info.size(); i++)
        {
            System.out.println(i+": disk_name="+vec_disk_info.get(i).disk_name+", disk_capacity="+vec_disk_info.get(i).disk_capacity+", disk_use_rate="+vec_disk_info.get(i).disk_use_rate);
        }

        SysInfo sys_info = new SysInfo();
        sys_info = device_info.GetSysInfo();
        System.out.println("system_type="+sys_info.system_type+", system_version="+sys_info.system_version+", hostname="+sys_info.hostname);

    }
};
