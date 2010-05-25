#ifndef TFTP_MODE_H_
#define TFTP_MODE_H_

#include <stdint.h>

#include "tftp_packet.h"

typedef enum {
	NETASCII=1,
	OCTET=2
} packet_mode;

/* External functions */
int16_t mode_to_chars(packet_read_write *reference, char *text);
int16_t chars_to_mode(packet_read_write *reference, char *text);
/* Internal functions */
int16_t move_right_insert(int16_t position, char charAdd, char *text);
int16_t move_right_put_after(int16_t position, char charAdd, char *text);
int16_t delete_character(int16_t position, char *text);

#endif /*TFTP_MODE_H_*/
