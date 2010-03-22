#ifndef TFTP_MODE_H_
#define TFTP_MODE_H_

#include <stdint.h>

typedef enum {
	NETASCII=1,
	OCTET=2
} packet_mode;

uint16_t mode_to_chars(packet_mode *mode, char *buff);
uint16_t chars_to_mode(packet_mode *mode, char *buff);
uint16_t encode_mode(packet_mode *mode, char *buff);

#endif /*TFTP_MODE_H_*/
