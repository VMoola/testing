#FILENAME : name of the module we are building (no .c)
#K : Build directory of the kernel, relative to $(HOME)/kernel/
#	- Expects "make modules" and "make modules_prepare" already done
#D : Where the module.ko is copied to.  Typically the shared guest dir,
#	relative to $(HOME)/testing/

FILENAME = hello
K = compiled
D = mkosi.extra/modules
#can be overwritten through the command line "FILENAME=modname"

obj-m += $(FILENAME).o
#simply add more objects here to build more modules
#for multi-file modules, simply 'chain' things:
# ab-objs := a.o b.o = middle man object
# obj-m += ab.o = combined object name for module


#it's likely better to write a script so its all cleanly
#moved to the shared mount directory, or modify the cp command below
PWD := $(CURDIR)
TARGET := $(HOME)/testing/$(D)/$(FILENAME)

all:
	make -C $(HOME)/kernel/$(K) M=$(PWD) modules
	#copy file to shared mount for easy insertion in QEMU
	cp $(FILENAME).ko $(TARGET).ko

clean:
	make -C $(HOME)/kernel/$(K) M=$(PWD) clean
	rm $(TARGET).ko

help:
	@printf "FILENAME=Module_name\nK=Kernel_build_dir\nD=Module_Destination\n"
	@printf "See file for more details\n"
