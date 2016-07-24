obj-m += bridgebreak_out.o 
bridgebreak_out-y = bridgebreak.o 


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
