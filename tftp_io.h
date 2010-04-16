#ifndef TFTP_IO_H_
#define TFTP_IO_H_

#include <stdint.h>
#include <sys/types.h>

int16_t read_bytes(const char *filename, off_t desiredpos, char *buff, uint16_t bufflen);
int16_t write_bytes(const char *filename, off_t desiredpos, const char *buff, uint16_t bufflen);

#endif /*TFTP_IO_H_*/