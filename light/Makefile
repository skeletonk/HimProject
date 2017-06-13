KTARGET := light

obj-m += $(KTARGET).o
#linux 3.11.0-15-generic
#KDIR := /lib/modules/$(shell uname -r)/build 
#kernel-mds2450-3.0.22
KDIR := /work/kernel-mds2450-3.0.22
CC := arm-none-linux-gnueabi-gcc
TARGET := light_app
NFSDIR := /nfs/rootfs/home
all:
	make -C$(KDIR) SUBDIRS=$(PWD) modules
	$(CC) $(TARGET).c -o $(TARGET)
	cp $(TARGET) $(NFSDIR)
	cp $(KTARGET).ko $(NFSDIR)
clean:
	make -C$(KDIR) SUBDIRS=$(PWD) clean
	rm $(TARGET)
