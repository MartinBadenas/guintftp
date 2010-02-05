#include "tftp_packet.h"
#include "tftp_auxi.h"
#include "tftp_log.h"

int guess_packet_type(char *buff, int bufflen, enum packet_type *type);
	if(bufflen < 2) {
		log_error("Invalid buffer!");
		return -1;
	}
	type = buff[2];
	return 0;
}

// reads "buff" and inits "packet"
int buff_to_packet_read_write(char *buff, int bufflen, struct packet_read_write *packet) {
	int strLen, strModeLen;
	
	strLen = 2;
	packet->op = (short) buff[1];
	// el op de los paquetes write y read es 1 y 2
	if(packet->op != 1 && packet->op != 2) {
		log_error("Invalid packet type!");
		return -1;
	}
	while(buff[strLen] != '\0' && strLen < bufflen) {
		strLen++;
	}
	if(buff[strLen] != '\0') {
		log_error("Invalid buffer!");
		return -1;
	}
	substr(packet->filename, buff, 2, strLen);
	strModeLen = strLen + 2;
	strLen = 0;
	while((buff[strModeLen + strLen] != '\0') && (strModeLen + strLen < bufflen)) {
		strLen++;
	}
	if(buff[strModeLen + strLen] != '\0') {
		log_error("Invalid buffer!");
		return -1;
	}
	substr(packet->mode, buff, strModeLen, strLen);
	return 0;
}

int buff_to_packet_data(char *buff, int bufflen, struct packet_data *packet) {
	if(bufflen < 4) {
		log_error("Buffer too short!");
		return -1;
	}
	packet->op = (short) buff[1];
	if(packet->op != 3) {
		log_error("Invalid packet type!");
		return -1;
	}
	packet->datalen = bufflen - 4;
	if(packet->datalen > 512) {
		log_error("Buffer too long");
		return -1;
	}
	packet->block = 0;
	if(packet->datalen > 0) {
		memcpy(
	}
	return 0;
}

int buff_to_packet_ack(char *buff, int bufflen, struct packet_ack *packet) {
	return 0;
}

int buff_to_packet_error(char *buff, int bufflen, struct packet_error *packet) {
	return 0;
}

int packet_read_write_to_bytes(char *buffer, struct packet_read_write *packet) {
	return 0;
}

int packet_data_to_bytes(char *buffer, struct packet_data *packet) {
	return 0;
}

int packet_ack_to_bytes(char *buffer, struct packet_ack *packet) {
	return 0;	
}

int packet_error_to_bytes(char *buffer, struct packet_error *packet) {
	return 0;
}