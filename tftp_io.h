#ifndef TFTP_IO_H_
#define TFTP_IO_H_

#include <stdint.h>

uint16_t read_block(const char *filename, uint16_t block, char *buff);
uint16_t write_block(const char *filename, uint16_t block, const char *buff, uint16_t bufflen);

#endif /*TFTP_IO_H_*/