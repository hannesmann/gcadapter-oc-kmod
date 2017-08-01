obj-m += gcadapter_oc.o
ccflags-y := -std=gnu99

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
