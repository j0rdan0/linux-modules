#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#define FINIT_SYSCALL 313
#define MODULE_DEL_SYSCALL 176


struct module_info {
	char filename[256];
	char name[256];
};

bool load_module(char*);
bool unload_module(char*);
// to add parameters for only loading/unloading the module, otherwise I can`t have this fully working for complex shit
// also, to modify script to receive kernel module parameters, for not having to hardcode shit inside the code
bool find_module(struct module_info*);

int main(int argc, char* argv[]) {
    int s_value = 10;
	if (argc > 1) {
		s_value = atoi(argv[1]);
	}
	struct module_info info;
	if(!find_module(&info)) {
		printf("[*] Failed finding module in current dir, exiting");
		return -1;
	}
	if (argc > 2) {
			if (strcmp(argv[2],"load") == 0) {
				if(load_module(info.filename)) {
					printf("[*]Failed loading module\n");
					perror("err");
					return -1;
				}
				else {
					printf("[*]Loaded module\n");
					return 0;
				}
			
			}

			else if (strcmp(argv[2],"unload") == 0) {
				if (!unload_module(info.name)) {
        				printf("[*]Unloaded module\n");
					return 0;
        			}
        			else {
                			printf("[*] Failed unloading module\n");
                			perror("err");
                			return -1;

        			}
			}	
			else {
				printf("usage: %s\n <timeout> <load/unload>\n",argv[0]);
				return -1;
			}
		}

    if(load_module(info.filename)) {
        printf("[*]Failed loading module\n");
        perror("err");
        return -1;
        }
    else {
        printf("[*]Loaded module\n");
	}
    sleep(s_value);
    if (!unload_module(info.name)) {
        printf("[*]Unloaded module\n");
	}
	else {
		printf("[*] Failed unloading module\n");
		perror("err");
		return -1;
	
	}
        return 0;
}

bool load_module(char* filename) {
//finit_module(int fd, const char* param_values,int flags)
        int fd = open("skeleton.ko",O_RDONLY);
	if ( fd == -1) {
		perror("failed opening file:");
		return false;
	}
        char null[] ="";
	// will need to modify null to get the passed module params
        long ret = syscall(FINIT_SYSCALL,fd,null,0);
        close(fd);
        return (!ret) ? false: true;
}

bool unload_module(char* module) {
        long ret = syscall(MODULE_DEL_SYSCALL,module,O_NONBLOCK);
	return (!ret) ? false: true;
}


// finds only first module in cwd, if more than one, will only load the one if alphabetical order
bool find_module(struct module_info* info) {
	DIR* cwd = opendir(".");
	if(cwd == NULL) {
		printf("[*] Failed getting handle to curdir");
		return false;
	}
	char* sufix = ".ko";
	struct dirent* entry;
	bool found = false;
	while( (entry = readdir(cwd)) != NULL ) {
		if(strstr(entry->d_name,sufix) != NULL) {
			strcpy(info->filename,entry->d_name);
			char* name = strtok(entry->d_name,".");
			strcpy(info->name,name);
			found = true;
			break;
		}
	}
	if(!found) {
		printf("[*] No module was found in current directory");
		closedir(cwd);
		return false;
	}
	closedir(cwd);
	return true;
}


