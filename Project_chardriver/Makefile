obj-m := main_1.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc  -o ioctlusr ioctl.c
