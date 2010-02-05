#include "tftp_packet.h"
#include "tftp_auxi.h"
#include "tftp_log.h"

// reads "buff" and inits "packet"
int buff_to_packet_read_write(char *buff, int bufflen, struct packet_read_write *packet) {
	int strLen, strModeLen;
	
	strLen = 2;
	packet->op = (short) buff[1];
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