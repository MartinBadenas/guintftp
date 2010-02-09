#ifndef TFTP_MODE_H_
#define TFTP_MODE_H_

typedef enum {
	NETASCII=1,
	OCTET=2
} packet_mode;

int mode_to_chars(paquet_mode *mode, char *buff);
int chars_to_mode(paquet_mode *mode, char *buff);
int encode_mode(paquet_mode *mode, char *buff);

#endif /*TFTP_MODE_H_*/
