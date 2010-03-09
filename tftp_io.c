#include "tftp_io.h"
#include "tftp_log.h"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int read_block(const char *filename, int block, int block_size, char *buff) {
	int fd;
	off_t pos, desiredpos;
	ssize_t num_bytes = 0;
	
	fd = open(filename, O_RDONLY);
	if(fd == -1) {
		log_error("Failed opening file");
		return -1;
	}
	desiredpos = block*block_size;
	pos = lseek(fd, desiredpos, SEEK_SET);
	if(pos == -1 || pos != desiredpos) {
		log_error("lseek error");
		return -1;
	}
	while(num_bytes += read(fd, &buff[num_bytes], block_size) > 0);
	
	if(num_bytes == -1 || num_bytes != block_size) {
		log_error("Failed reading file");
		return -1;
	}
	if(close(fd) == -1) {
		log_error("Failed closing file");
		return -1;
	}
	return 0;
}
int write_block(const char *filename, int block, int block_size, char *buff) {
	return -1;
}
