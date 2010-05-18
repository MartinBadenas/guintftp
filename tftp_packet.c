#include "tftp_packet.h"
#include "tftp_log.h"
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>

/* Devuelve el tipo de paquete del array */
int16_t guess_packet_type(char *buff, uint16_t bufflen, packet_type *type) {
	if(bufflen < 2) {
		printf("<%d>\n", bufflen);
		log_error("Invalid buffer (guess_packet_type)!");
		return -1;
	}
	if(buff[1] < 1 || buff[1] > 5) {
		log_error("invalid opcode");
		return -1;
	}
	*type = buff[1];
	return 0;
}

/* Crea un packet_read_write a partir del array de chars */
int16_t buff_to_packet_read_write(char *buff, uint16_t bufflen, packet_read_write *packet) {
	int i, modelen;
	
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
	/* si uno de los dos en null, faltan saltos de línea */
	if(packet->mode == NULL || packet->filename == NULL) {
		log_error("Invalid packet!");
		return -1;
	}
	modelen = strlen(packet->mode);
	for(i = 0; i < modelen; i++) {
		packet->mode[i] = tolower(packet->mode[i]);
	}
	if((modelen != 8 && modelen != 5) || (strncmp(packet->mode, "netascii", 8) != 0 && strncmp(packet->mode, "octet", 5) != 0)) {
		log_error("Invalid packet!");
		return -1;
	}
	return 0;
}

/* Crea un packet_data a partir del array de chars */
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

/* Crea un packet_ack a partir del array de chars */
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

/* Crea un packet_error a partir del array de chars */
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

/* Crea un array de chars a partir de un packet_data */
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

/* Crea un array de chars a partir de un packet_ack */
int16_t packet_ack_to_bytes(char **buffer, packet_ack *packet) {
	if(packet->op != ACK) {
		log_error("invalid packet type");
		return -1;
	}
	packet->op = htons(packet->op);
	packet->block = htons(packet->block);
	*buffer = (char*) packet;
	return 4;	
}

/* Crea un array de chars a partir de un packet_error */
int16_t packet_error_to_bytes(char **buffer, packet_error *packet) {
	int len;
	
	if(ERROR != packet->op) {
		log_error("invalid packet type");
		return -1;
	}
	if(packet->error_code < 0 || packet->error_code > 7) {
		log_error("Invalid error code!");
		return -1;
	}
	/* TODO: strlen no es seguro */
	len = 5 + strlen(packet->errmsg);
	if(len < MIN_ERROR_SIZE) {
		log_error("Bad error packet");
		return -1;
	}
	packet->op = htons(packet->op);
	packet->error_code = htons(packet->error_code);
	*buffer = (char*) packet;
	return len;
}

int16_t error_code(uint16_t error_code, char **string, uint16_t *len) {
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
	*string = error_codes[error_code];
	*len = strlen(error_codes[error_code]);
	return 0;
}