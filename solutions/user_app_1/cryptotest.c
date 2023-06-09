#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>


#define OFFSET_INPUT_LOW 0x0 
#define OFFSET_INPUT_HIGH 0x4
#define OFFSET_KEY_LOW 0x8
#define OFFSET_KEY_HIGH 0xC
#define OFFSET_CONTROL_IN 0x10
#define OFFSET_OUTPUT_LOW 0x0
#define OFFSET_OUTPUT_HIGH 0x4
#define OFFSET_CONTROL_OUT 0x8

#define LM_MAGIC_NUMBER 0XFF
#define ENCRYPT _IOW(LM_MAGIC_NUMBER, 1, int)
#define DECRYPT _IOW(LM_MAGIC_NUMBER, 2, int)
#define RESTART _IOW(LM_MAGIC_NUMBER, 3, int)
#define DONE _IOW(LM_MAGIC_NUMBER, 4, int)

int main() {

	int fd = open ("/dev/CRYPTOCORE",O_RDWR);
	long r;
	if (fd < 0) {
		perror ("Unable to open device");
		return 0;
	}

	unsigned int arg;

	uint32_t buf;
	buf = 0x51554954;
	r = pwrite (fd, &buf, sizeof(uint32_t), (off_t)OFFSET_INPUT_LOW);
	

	buf = 0x4E455652;
	r = pwrite (fd, &buf, sizeof(uint32_t), (off_t)OFFSET_INPUT_HIGH);
	
	
	buf = 0x49534142;
	r = pwrite (fd, &buf, sizeof(uint32_t), (off_t)OFFSET_KEY_LOW);
	

	buf = 0x4B415348;
	r = pwrite (fd, &buf, sizeof(uint32_t), (off_t)OFFSET_KEY_HIGH);
	
	r =  ioctl(fd, ENCRYPT, &arg);

	int flag = 0;
	while (flag == 0) {
		if (ioctl(fd, DONE, &arg) == 1){
			flag = 1;
			printf("DEBUG: waiting for core to be done \n");
		}
	}	
	pread(fd,&buf,sizeof(uint32_t),(off_t)OFFSET_OUTPUT_LOW);
	printf("OUTPUT LOW IS: %x\n", ((unsigned int)buf));
	pread(fd,&buf,sizeof(uint32_t),(off_t)OFFSET_OUTPUT_HIGH);
	printf("OUTPUT HIGH IS: %x\n", ((unsigned int)buf));
	pread(fd,&buf,sizeof(uint32_t),(off_t)OFFSET_CONTROL_OUT);
	printf("OUTPUT CONTROL IS: %x\n", ((unsigned int)buf));
	
	close(fd);
	return 1;
}
