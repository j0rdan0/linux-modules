obj-m += networking.o
CFLAGS_networking.o := -DDEBUG

all:      
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
loader:
	gcc -o load-module load-module.c
run:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o load-module load-module.c
	./load-module