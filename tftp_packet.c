#include "tftp_packet.h"
#include "tftp_log.h"
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>

int16_t guess_packet_type(char *buff, uint16_t bufflen, packet_type *type) {
	if(bufflen < 2) {
		log_error("Invalid buffer!");
		return -1;
	}
	*type = buff[1];
	return 0;
}

/* reads "buff" and inits "packet" */
int16_t buff_to_packet_read_write(char *buff, uint16_t bufflen, packet_read_write *packet) {
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
	
	for(i = 4; i < bufflen; i++) {
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
	for(i = 0; i < strlen(packet->mode); i++) {
		packet->mode[i] = tolower(packet->mode[i]);
	}
	if(strncmp(packet->mode, "netascii", 8)!=0 && strncmp(packet->mode, "octet", 5)!=0) {
		log_error("Invalid packet!");
		return -1;
	}
	return 0;
}

int16_t buff_to_packet_data(char *buff, uint16_t bufflen, packet_data **packet) {
	packet_data *pck;
	
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
	*packet = (packet_data*) buff;
	pck = *packet;
	pck->block = ntohs(pck->block);
	pck->op = ntohs(pck->op);
	if(pck->block <= 0) {
		log_error("Invalid block number!");
		return -1;
	}
	return 0;
}

int16_t buff_to_packet_ack(char *buff, uint16_t bufflen, packet_ack **packet) {
	packet_ack *pck;
	
	if(bufflen != ACK_SIZE) {
		log_error("Incorrect buffer size!");
		return -1;
	}
	if(buff[1] != ACK) {
		log_error("Invalid packet type!");
		return -1;
	}
	*packet = (packet_ack*) buff;
	pck = *packet;
	pck->op = ntohs(pck->op);
	pck->block = ntohs(pck->block);
	return 0;
}

int16_t buff_to_packet_error(char *buff, uint16_t bufflen, packet_error **packet) {
	packet_error *pck;
	int strLen;
	
	if(bufflen < MIN_ERROR_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	if(buff[1] != ERROR) {
		log_error("Invalid packet type!");
		return -1;
	}
	if(buff[3] < 0 || buff[3] > 7) {
		log_error("Invalid error code!");
		return -1;
	}
	strLen = 4;
	while(buff[strLen] != '\0' && strLen < bufflen) {
		strLen++;
	}
	if(buff[strLen] != '\0') {
		log_error("Not null terminated string!");
		return -1;
	}
	
	*packet = (packet_error*) buff;
	pck = *packet;
	pck->op = ntohs(pck->op);
	pck->error_code = ntohs(pck->error_code);
	return 0;
}

int16_t packet_data_to_bytes(char **buffer, packet_data *packet) {
	if(packet->op != DATA) {
		log_error("invalid packet type");
		return -1;
	}
	packet->op = htons(packet->op);
	packet->block = htons(packet->block);
	*buffer = (char*) packet;
	return 0;
}

int16_t packet_ack_to_bytes(char *buffer, const packet_ack *packet) {
	buffer[0] = (char) 0;
	buffer[1] = (char) packet->op;
	buffer[2] = (char) packet->block >> 8;
	buffer[3] = (char) packet->block & 0xff;
	return 4;	
}

int16_t packet_error_to_bytes(char *buffer, const packet_error *packet) {
	int len;
	int errmsglen;
	
	errmsglen = strlen(packet->errmsg) + 1;
	len = 4 + errmsglen;
	if(len < MIN_ERROR_SIZE) {
		log_error("Bad error packet");
		return -1;
	}
	buffer[0] = (char) 0;
	buffer[1] = (char) packet->op;
	buffer[2] = (char) 0;
	buffer[3] = (char) packet->error_code;
	strncpy(&buffer[4], packet->errmsg, errmsglen);
	return len;
}

int16_t error_code(uint16_t error_code, char *string, uint16_t *len) {
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