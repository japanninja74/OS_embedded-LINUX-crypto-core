#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

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

#define BUFLENGTH 200
#define output_path "output.txt"

int command_parser(int argc, char **argv, char *file_name, uint64_t * key, int * mode);

//-[d/e] [64bit key in exadecimal] [file or path to file txt]
int main(int argc, char **argv) {

	char text_file[BUFLENGTH];
	uint64_t key;
	int mode;
	
	//checking arguments
	int return_type = command_parser(argc, argv, text_file, &key, &mode);
	if (return_type != 0){
		fprintf(stderr, "error parsing arguments, exiting \n");
		return 1;
	}
	
	//opening device
	int fd = open ("/dev/CRYPTOCORE",O_RDWR);
	long r;
	if (fd < 0) {
		perror ("Unable to open device");
		return 1;
	}
	
	//opening text file
	int input_file = open (text_file,O_RDONLY);
	if (input_file < 0) {
		perror ("Unable to open input file");
		close(fd);
		return 1;
	}
	
	//opening output file
	int output_file = open (output_path,O_RDWR | O_CREAT | O_EXCL, 0666);
	if (output_file < 0) {
		perror ("Unable to create output file");
		close(fd);
		close(input_file);
		return 1;
	}
	
	//setting key
	unsigned int arg;
	pwrite (fd, (char*)&key, sizeof(uint32_t), (off_t)OFFSET_KEY_LOW);
	pwrite (fd, (char*)((size_t)&key + sizeof(uint32_t)), sizeof(uint32_t), (off_t)OFFSET_KEY_HIGH);
	
	int flag = 0;
	int flag_2 = 0;
	uint64_t buf = 0;
	while (flag == 0){
		ioctl(fd, RESTART, &arg);
		buf = 0;
		flag = read(input_file, (void *)&buf, sizeof(uint64_t));
		if (flag <= 0){
			flag = 1;
			printf("DEBUG: end of file reached \n");
		}else{
			flag = 0;
			//writing to core input
			pwrite (fd, (char *)&buf, sizeof(uint32_t), (off_t)OFFSET_INPUT_LOW);
			pwrite (fd, (char*)((size_t)&buf + sizeof(uint32_t)), sizeof(uint32_t), (off_t)OFFSET_INPUT_HIGH);
			//checking mode and calling ioctl
			if (mode == 0){
				ioctl(fd, ENCRYPT, &arg);
			}
			else{
				ioctl(fd, DECRYPT, &arg);
			}
			flag_2 = 0;
			//checking done flag
			while (flag_2 == 0) {
				if (ioctl(fd, DONE, &arg) == 1){
					flag_2 = 1;
					printf("DEBUG: waiting for core to be done \n");
				}
			}
			//reading from core output
			pread(fd,(char *)&buf,sizeof(uint32_t),(off_t)OFFSET_OUTPUT_LOW);
			pread(fd,(char*)((size_t)&buf + sizeof(uint32_t)),sizeof(uint32_t),(off_t)OFFSET_OUTPUT_LOW);
			//writing to output file
			write(output_file,(void*)buf,sizeof(uint64_t));
		}
	}
	
	printf("program has finished, exiting");
	close(fd);
	close(output_file);
	close(input_file);
	return 1;
}

int command_parser (int argc, char **argv, char *file_name, uint64_t * key, int * mode) {

	char mode_tmp[2];
	char file_tmp[BUFLENGTH];
	char key_tmp[17];
	
	//checking number of arguments
	if (argc != 4) {
		fprintf(stderr, "error, wrong number of arguments; arguments must be: -[d/e] [64bit key in exadecimal] [file or path to file txt] \n");
		return 1;
	}
	
	//command regex
	regex_t command_rgx;
	if (regcomp(&command_rgx, "^-[de]$", REG_EXTENDED)){
		fprintf(stderr, "could not compile regex command\n");
		regfree(&command_rgx);
		return 1;
	}
	//key regex
	regex_t key_rgx;
	if (regcomp(&key_rgx, "^[[:xdigit:]]{16}$", REG_EXTENDED)){
		fprintf(stderr, "could not compile regex key\n");
		regfree(&command_rgx);
		regfree(&key_rgx);
		return 1;
	}
	//text file regex
	regex_t text_file_rgx;
	if (regcomp(&text_file_rgx, "^[[:alnum:]_/]{1,}.txt$", REG_EXTENDED)){
		fprintf(stderr, "could not compile regex text_file\n");
		regfree(&command_rgx);
		regfree(&key_rgx);
		regfree(&text_file_rgx);
		return 1;
	}
	
	//checking mode argument
	if (regexec(&command_rgx,argv[1],0,NULL,0) == 0){
		if (strncmp(argv[1],"-e",2)) {
			*mode = 0;
		}
		else {
			*mode = 1;
		}
	}
	else {
		fprintf(stderr, "error, mode argument does not match regex \n");
		regfree(&command_rgx);
		regfree(&key_rgx);
		regfree(&text_file_rgx);
		return 1;
	}
	//checking key argument
	if (regexec(&key_rgx,argv[2],0,NULL,0) == 0){
		*key = strtoull(argv[2], NULL, 16);
	}
	else {
		fprintf(stderr, "error, key argument does not match regex \n");
		regfree(&command_rgx);
		regfree(&key_rgx);
		regfree(&text_file_rgx);
		return 1;
	}
	//checking filename argument
	if (regexec(&text_file_rgx,argv[3],0,NULL,0) == 0){
		if (strlen(argv[3]) <= (BUFLENGTH - 1) ) {
			strncpy(file_name, argv[3], BUFLENGTH);
		}
		else {
			fprintf(stderr, "error, filename argument exceeds %d characters\n", BUFLENGTH);
			regfree(&command_rgx);
			regfree(&key_rgx);
			regfree(&text_file_rgx);
			return 1;
		}
	}
	else {
		fprintf(stderr, "error, filename argument does not match regex \n");
		regfree(&command_rgx);
		regfree(&key_rgx);
		regfree(&text_file_rgx);
		return 1;
	}
	
	return 0;
}
