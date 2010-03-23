#include "tftp_io.h"
#include "tftp_packet.h"
#include "tftp_log.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int close_file(int fd) {
	if(close(fd) == -1) {
		log_error("Failed closing file");
		return -1;
	}
	return 0;
}

uint16_t read_block(const char *filename, uint16_t block, char *buff) {
	int fd;
	off_t desiredpos;
	off_t pos;
	ssize_t num_bytes = 0;
	
	--block;
	fd = open(filename, O_RDONLY);
	if(fd == -1) {
		log_error("Failed opening file");
		return -1;
	}
	desiredpos = block * DATA_SIZE;
	pos = lseek(fd, desiredpos, SEEK_SET);
	if(pos == -1 || pos != desiredpos) {
		log_error("lseek error");
		return -1;
	}
	while(num_bytes += read(fd, &buff[num_bytes], DATA_SIZE) > 0);
	
	if(num_bytes == -1) {
		log_error("Failed reading file");
		return -1;
	}
	if(close_file(fd) == -1)
		num_bytes = -1;
	
	return num_bytes;
}
uint16_t write_block(const char *filename, uint16_t block, const char *buff, uint16_t bufflen) {
	int fd;
	ssize_t num_bytes = 0, error;
	
	--block;
	fd = open(filename, O_WRONLY | O_APPEND | O_CREAT);
	if(fd == -1) {
		log_error("Failed opening file");
		return -1;
	}
	while(error != -1 && num_bytes != bufflen) {
		error = write(fd, &buff[num_bytes], bufflen);
		num_bytes += error;
	}
	
	if(error == -1) {
		log_error("Failed writing file");
		return -1;
	}
	
	if(close_file(fd) == -1)
		return -1;
	else
		return 0;
}
