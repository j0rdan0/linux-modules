obj-m += skeleton.o
loader:
	gcc -o load-module load-module.c
all:      
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

run:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o load-module load-module.c
	./load-module