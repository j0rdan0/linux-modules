#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc,char* argv[]) {
	if (argc < 2) {
		puts("usage: main <string>");
		return -1;
	}
	int fd = open("/dev/test-dev",O_RDWR);
	char buff[24];
	ssize_t s = read(fd,buff,23);
	printf("%s\n",buff);
	char*msg = argv[1];
	s = write(fd,msg,strlen(msg));
	if(s < 0) {
		perror("err");
	}
	else {
		puts("wrote to file");
	}
	close(fd);

}
