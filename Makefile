obj-m += mod1.o
obj-m += mod_NL.o
obj-m += mod_PAGE_SIZE.o
obj-m += mod_lin_addr.o
obj-m += mod_PAGE_WALK.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
