#include "tftp_packet.h"
#include "tftp_auxi.h"
#include "tftp_log.h"

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
	int strLen, posicion;
	
	packet->op = (short) buff[1];
	// el op de los paquetes write y read es 1 y 2
	if(packet->op != 1 && packet->op != 2) {
		log_error("Invalid packet type!");
		return -1;
	}
	posicion = 2;//posicion en la que busca el nombre del archivo
	strLen = 0;
	while(buff[posicion + strLen] != '\0' && (posicion + strLen) < bufflen) {
		strLen++;
	}
	if(buff[posicion + strLen] != '\0') {
		log_error("Invalid buffer!");
		return -1;
	}
	substr(packet->filename, buff, posicion, strLen);
	posicion += strLen + 1;//posicion en la que busca el modo
	strLen = 0;
	while((buff[posicion + strLen] != '\0') && (posicion + strLen < bufflen)) {
		strLen++;
	}
	if(buff[posicion + strLen] != '\0') {
		log_error("Invalid buffer!");
		return -1;
	}
	substr(packet->mode, buff, posicion, strLen);
	return 0;
}

int buff_to_packet_data(char *buff, int bufflen, packet_data *packet) {
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
		memcopy(packet->data, buff, 4, packet->datalen);
	}
	return 0;
}

int buff_to_packet_ack(char *buff, int bufflen, packet_ack *packet) {
	if(4 > bufflen) {
		log_error("Buffer too short!");
		return -1;
	}
	else if(4 < bufflen) {
		log_error("Buffer too long!");
		return -1;
	}
	packet->op = (short) buff[1];
	packet->block = ((((short) buff[2]) * 10) + ((short) buff[3]));
	return 0;
}

int buff_to_packet_error(char *buff, int bufflen, packet_error *packet) {
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