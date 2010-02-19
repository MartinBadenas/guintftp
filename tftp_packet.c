#include "tftp_packet.h"
#include "tftp_log.h"
#include <string.h>

int guess_packet_type(char *buff, int bufflen, packet_type *type) {
	if(bufflen < 2) {
		log_error("Invalid buffer!");
		return -1;
	}
	*type = buff[1];
	return 0;
}

// reads "buff" and inits "packet"
int buff_to_packet_read_write(char *buff, int bufflen, packet_read_write *packet) {
	int strLen, pos;
	char *pnt;
	
	packet->op = (short) buff[1];
	// el op de los paquetes write y read es 1 y 2
	if(packet->op != RRQ && packet->op != WRQ) {
		log_error("Invalid packet type!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	if(bufflen < MIN_READ_WRITE_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	pos = 2;//posicion en la que busca el nombre del archivo
	strLen = 0;
	while(buff[pos + strLen] != '\0' && (pos + strLen) < bufflen) {
		strLen++;
	}
	if(buff[pos + strLen] != '\0') {
		log_error("Invalid buffer!");
		return -1;
	}
	pnt = &buff[pos];
	strncpy(packet->filename, pnt, strLen);
	pos += strLen + 1;//posicion en la que busca el modo
	strLen = 0;
	while((buff[pos + strLen] != '\0') && (pos + strLen < bufflen)) {
		strLen++;
	}
	if(buff[pos + strLen] != '\0') {
		log_error("Invalid buffer!");
		return -1;
	}
	pnt = &buff[pos];
	strncpy(packet->mode, pnt, strLen);
	return 0;
}

int buff_to_packet_data(char *buff, int bufflen, packet_data *packet) {
	char *pnt;
	
	if(bufflen < MIN_DATA_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	packet->op = (short) buff[1];
	if(packet->op != DATA) {
		log_error("Invalid packet type!");
		return -1;
	}
	packet->datalen = bufflen - 4;
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	packet->block = 0;
	if(packet->datalen > 0) {
		pnt = &buff[4];
		memcpy(packet->data, pnt, packet->datalen*sizeof(char));
	}
	return 0;
}

int buff_to_packet_ack(char *buff, int bufflen, packet_ack *packet) {
	if(bufflen != ACK_SIZE) {
		log_error("Incorrect buffer size!");
		return -1;
	}
	packet->op = (short) buff[1];
	if(packet->op != ACK) {
		log_error("Invalid packet type!");
		return -1;
	}
	packet->block = ((((short) buff[2]) * 10) + ((short) buff[3]));
	return 0;
}

int buff_to_packet_error(char *buff, int bufflen, packet_error *packet) {
	int strLen;
	char *pnt;
	
	if(bufflen < MIN_ERROR_SIZE) {
		log_error("Buffer too short!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		log_error("Buffer too long");
		return -1;
	}
	packet->op = (short) buff[1];
	if(packet->op != ERROR) {
		log_error("Invalid packet type!");
		return -1;
	}
	packet->error_code = (short) buff[3];
	strLen = 4;
	while(buff[strLen] != '\0') {
		strLen++;
	}
	pnt = &buff[4];
	strncpy(packet->err_msg, pnt, strLen);
	return 0;
}

int packet_read_write_to_bytes(char *buffer, packet_read_write *packet) {
	
	return 0;
}

int packet_data_to_bytes(char *buffer, packet_data *packet) {
	return 0;
}

int packet_ack_to_bytes(char *buffer, packet_ack *packet) {
	return 0;	
}

int packet_error_to_bytes(char *buffer, packet_error *packet) {
	return 0;
}

int error_code(int error_code, char *string, int *len) {
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