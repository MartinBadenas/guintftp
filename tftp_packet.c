#include "tftp_packet.h"
#include "tftp_auxi.h"

void read_packet_read_write(char *buff, int bufflen, struct packet_read_write *paquet) {
	int strLen, strModeLen;
	
	strLen = 2;
	paquet->op = (short) buff[1];
	while(buff[strLen] != '\0' && strLen < bufflen) {
		strLen++;
	}
	substr(paquet->filename, buff, 2, strLen);
	strModeLen = strLen + 2;
	strLen = 0;
	while((buff[strModeLen + strLen] != '\0') && (strModeLen + strLen < bufflen)) {
		strLen++;
	}
	substr(paquet->mode, buff, strModeLen, strLen);
}

void read_packet_data(char *buff, int bufflen, struct packet_data *paquet) {
}

void read_packet_ack(char *buff, int bufflen, struct packet_ack *paquet) {
}

void read_packet_error(char *buff, int bufflen, struct packet_error *paquet) {
}

int write_packet_read_write(char *buffer, struct packet_read_write *paquet) {
	return 0;
}

int write_packet_data(char *buffer, struct packet_data *paquet) {
	return 0;
}

int write_packet_ack(char *buffer, struct packet_ack *paquet) {
	return 0;	
}

int write_packet_error(char *buffer, struct packet_error *paquet) {
	return 0;
}