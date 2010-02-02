#ifndef TFTP_IO_H_
#define TFTP_IO_H_

int read_block(char *filename, int block, int block_size, char *buff);
int write_block(char *filename, int block, int block_size, char *buff);

#endif /*TFTP_IO_H_*/