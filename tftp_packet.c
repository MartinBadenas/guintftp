#include "tftp_packet.h"
#include "tftp_log.h"
#include <string.h>
#include <stdio.h>

int guess_packet_type(char *buff, uint16_t bufflen, packet_type *type) {
	if(bufflen < 2) {
		log_error("Invalid buffer!");
		return -1;
	}
	*type = buff[1];
	return 0;
}

/* reads "buff" and inits "packet" */
int buff_to_packet_read_write(char *buff, uint16_t bufflen, packet_read_write *packet) {
	int i;
	
	/* el op de los paquetes write y read es 1 y 2 */
	if(buff[1] != RRQ && buff[1] != WRQ) {
		log_error("Invalid packet type!");
		return -1;
	}
	packet->op = buff[1];
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	if(bufflen < MIN_READ_WRITE_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	packet->filename = NULL;
	packet->mode = NULL;
	for(i = 0; i < bufflen; i++) {
		if(buff[i] == '\0') {
			if(packet->mode != NULL) {
				packet->filename = &buff[2];
				break;
			} else {
				packet->mode = &buff[i + 1];
			}
		}
	}
	if(packet->mode == NULL || packet->filename == NULL) {
		log_error("Invalid packet!");
		return -1;
	}
	return 0;
}

int buff_to_packet_data(char *buff, uint16_t bufflen, packet_data *packet) {
	if(bufflen < MIN_DATA_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	if(buff[1] != DATA) {
		log_error("Invalid packet type!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	packet->block = *&buff[2];
	if(packet->block <= 0) {
		log_error("Invalid block number!");
		return -1;
	}
	packet->op = buff[1];
	packet->datalen = bufflen - 4;
	packet->data = NULL;
	if(packet->datalen > 0) {
		packet->data = &buff[4];
	}
	return 0;
}

int buff_to_packet_ack(char *buff, uint16_t bufflen, packet_ack *packet) {
	if(bufflen != ACK_SIZE) {
		log_error("Incorrect buffer size!");
		return -1;
	}
	packet->op = (unsigned short) buff[1];
	if(buff[1] != ACK) {
		log_error("Invalid packet type!");
		return -1;
	}
	
	packet->block = ((short) buff[2]) << 8;
	packet->block = packet->block | buff[3];
	return 0;
}

int buff_to_packet_error(char *buff, uint16_t bufflen, packet_error *packet) {
	int strLen;
	const char *pnt;
	
	if(bufflen < MIN_ERROR_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	packet->op = (unsigned short) buff[1];
	if(buff[1] != ERROR) {
		log_error("Invalid packet type!");
		return -1;
	}
	packet->error_code = (short) buff[3];
	strLen = 4;
	/* FIXME: check bufflen limit */
	while(buff[strLen] != '\0') {
		strLen++;
	}
	pnt = &buff[4];
	strncpy(packet->errmsg, pnt, strLen+1);
	return 0;
}

int packet_data_to_bytes(char *buffer, const packet_data *packet) {
	int len;
	
	len = MIN_DATA_SIZE + packet->datalen;
	memset(buffer, 0, len);
	buffer[1] = (char) packet->op;
	buffer[2] = (char) packet->block >> 8;
	buffer[3] = (char) packet->block & 0xff;
	memcpy(&buffer[4], packet->data, packet->datalen);
	return len;
}

int packet_ack_to_bytes(char *buffer, const packet_ack *packet) {
	int len = 4;
	
	memset(buffer, 0, len);
	buffer[1] = (char) packet->op;
	buffer[2] = (char) packet->block >> 8;
	buffer[3] = (char) packet->block & 0xff;
	return len;	
}

int packet_error_to_bytes(char *buffer, const packet_error *packet) {
	int len;
	int errmsglen;
	char *pnt;
	
	errmsglen = strlen(packet->errmsg) + 1;
	len = 4 + errmsglen;
	if(len < MIN_ERROR_SIZE) {
		log_error("Bad error packet");
		return -1;
	}
	memset(buffer, 0, len);
	buffer[1] = (char) packet->op;
	buffer[3] = (char) packet->error_code;
	pnt = &buffer[4];
	strncpy(pnt, packet->errmsg, errmsglen);
	return len;
}

int error_code(uint16_t error_code, char *string, uint16_t *len) {
	static char *error_codes[7] = {
	"File not found.",
	"Access violation.",
	"Disk full or allocation exceeded.",
	"Illegal TFTP operation.",
	"Unknown transfer ID.",
	"File already exists.",
	"No such user."
	};
	error_code--;
	if(error_code > 6 || error_code < 0) {
		log_error("invalid error_code");
		return -1;
	}
	string = error_codes[error_code];
	*len = strlen(string);
	return 0;
}