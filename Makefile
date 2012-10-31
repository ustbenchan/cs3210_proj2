obj-m = syscalllog.o
KVERSION = $(shell uname -r)
COMPILE_OPTION = -g

all: unload syscalllog.o test_open

syscalllog.o: syscalllog.c
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

test_open: test_open.c
	gcc -o test_open test_open.c -lrt

load : syscalllog.o
	-/sbin/rmmod syscalllog
	/sbin/insmod syscalllog.ko

unload:
	-/sbin/rmmod syscalllog

puller: syscallLogPull.c
	gcc -o pullLog syscallLogPull.c

