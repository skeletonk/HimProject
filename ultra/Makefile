KTARGET := ultra

obj-m += $(KTARGET).o
#linux 3.11.0-15-generic
#KDIR := /lib/modules/$(shell uname -r)/build 
#kernel-mds2450-3.0.22
KDIR := /work/kernel
#KDIR := /work/kernel-mds2450-3.0.22
CC := arm-none-linux-gnueabi-gcc
TARGET := sk_app
NFSDIR := /nfs/rootfs/home
KOFDIR := /work/project/kofile
all:
	make -C$(KDIR) SUBDIRS=$(PWD) modules
#	$(CC) $(TARGET).c -o $(TARGET)
#	cp $(TARGET) $(NFSDIR)
	cp $(KTARGET).ko $(NFSDIR)

	cp $(KTARGET).ko $(KOFDIR)
clean:
	make -C$(KDIR) SUBDIRS=$(PWD) clean
#	rm $(TARGET)
