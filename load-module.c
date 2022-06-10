#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>

#define FINIT_SYSCALL 313
#define MODULE_DEL_SYSCALL 176

bool load_module();
void unload_module();

int main(int argc, char* argv[]) {
        int s_value = 10;
        if(load_module()) {
                printf("[*]Failed loading module\n");
                return -1;
        }
        else {
                printf("[*]Loaded module\n");
                if (argc > 1) {
                s_value = atoi(argv[1]);
}
                sleep(s_value);
                unload_module();
        }

        return 0;
}

bool load_module() {


//finit_module(int fd, const char* param_values,int flags)

        int fd = open("skeleton.ko",O_RDONLY);
        char null[] ="";
        long ret = syscall(FINIT_SYSCALL,fd,null,0);
        close(fd);
        return (!ret) ? false: true;

}

void unload_module() {
        char* MODULE = "skeleton";
        syscall(MODULE_DEL_SYSCALL,MODULE,O_NONBLOCK);
        printf("[*]Unloaded module\n");
}
