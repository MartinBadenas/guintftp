#include "tftp_io.h"
#include "tftp_packet.h"
#include "tftp_log.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int16_t close_file(int fd) {
	if(close(fd) == -1) {
		log_error("Failed closing file");
		return -1;
	}
	return 0;
}

int open_lseek(const char *filename, off_t desiredpos, int flags) {
	int fd;
	off_t pos;
	
	fd = open(filename, flags);
	if(fd == -1) {
		return -1;
	}
	pos = lseek(fd, desiredpos, SEEK_SET);
	if(pos == -1 || pos != desiredpos) {
		log_error("lseek error");
		return -1;
	}
	return fd;
}

int16_t read_bytes(const char *filename, off_t desiredpos, char *buff, uint16_t bufflen) {
	int fd;
	ssize_t num_bytes = 0, num_readed = 0;
	
	fd = open_lseek(filename, desiredpos, O_RDONLY);
	if(fd == -1) {
		log_error("Failed opening file");
		return -1;
	}
	while((num_readed = read(fd, &buff[num_bytes], bufflen)) > 0) {
		bufflen -= num_readed;
		num_bytes += num_readed;
	}
	
	if(num_bytes == -1 || bufflen != 0) {
		log_error("Failed reading file");
		return -1;
	}
	if(close_file(fd) == -1)
		num_bytes = -1;
	
	return num_bytes;
}
int16_t write_bytes(const char *filename, off_t desiredpos, const char *buff, uint16_t bufflen) {
	int fd;
	ssize_t num_bytes = 0, error, writelen;
	
	fd = open_lseek(filename, desiredpos, O_WRONLY | O_CREAT);
	if(fd == -1) {
		log_error("Failed opening file");
		return -1;
	}
	writelen = bufflen;
	while(error != -1 && num_bytes != bufflen) {
		error = write(fd, &buff[num_bytes], writelen);
		num_bytes += error;
		writelen -= error;
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
