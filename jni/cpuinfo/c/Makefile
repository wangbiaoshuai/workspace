all: libdevice_info.so

INCLUDE = -I./
CC = gcc
CXXFLAGS += ${INCLUDE} -std=gnu99

libdevice_info.so: com_vrv_cems_service_monitor_util_DeviceInfo.c
	${CC} -fPIC -shared -o $@ $^ ${CXXFLAGS}

.PHONY: clean

clean:
	${RM} -f /usr/lib64/libdevice_info.so libdevice_info.so
