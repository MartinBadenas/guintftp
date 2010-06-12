#ifndef TFTP_BLOCK_IO_H_
#define TFTP_BLOCK_IO_H_

#include <stdint.h>
#include <sys/types.h>

#include "tftp_packet.h"

typedef struct {
	char *filename;
	off_t filepos;
	int is_netascii;
	int lastwascr;
	int nextblockinsertlf;
} fd;

void openfd(fd *file, packet_read_write *packet);
int16_t read_block(fd *file, char *buff);
int16_t write_block(fd *file, char *buff, int16_t bufflen);
int16_t write_netascii_block(fd *file, char *buff, int16_t bufflen);

#endif /* TFTP_BLOCK_IO_H_ */
