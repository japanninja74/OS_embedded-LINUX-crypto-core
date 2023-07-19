#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>




#define LM_MAGIC_NUMBER 0XFF
#define EXAMPLE1 _IOW(LM_MAGIC_NUMBER, 1, int)
#define EXAMPLE2 _IOW(LM_MAGIC_NUMBER, 2, int)


int main() {

	int fd = open ("/dev/CRYPTOCORE",O_RDWR);
	long r;
	if (fd < 0) {
		perror ("Unable to open device");
		return 0;
	}
	
	// Example of how to use ioctl r =  ioctl(fd, EXAMPLE1, &arg);

	// SOME CODE
	
	close(fd);
	return 1;
}
