FILENAME = hello
#can be overwritten through the command line "FILENAME=modname"

obj-m += $(FILENAME).o
#simply add more objects here to build more modules
#for multi-file modules, simply 'chain' things:
# ab-objs := a.o b.o = middle man object
# obj-m += ab.o = combined object name for module


#it's likely better to write a script so its all cleanly
#moved to the shared mount directory, or modify the cp command below
PWD := $(CURDIR)
TARGET := $(HOME)/testing/mkosi.extra/modules/$(FILENAME)

all:
	make -C $(HOME)/kernel/compiled M=$(PWD) modules
	#copy file to shared mount for easy insertion in QEMU
	cp $(FILENAME).ko $(TARGET).ko

clean:
	make -C $(HOME)/kernel/compiled M=$(PWD) clean
	rm $(TARGET).ko
