INLCUDE = -I/devscan/include
LIBS = -Wl,-rpath /usr/local/devscan -L/usr/local/devscan -ldevscan

all:
	gcc -o dev_scan test.c $(LIBS)
clean:
	rm -rf dev_scan
