/*
 * Copyright (C) 2010 Dani Hernández Juárez, dhernandez0@gmail.com
 * Copyright (C) 2010 Martín Badenas Beltrán, martin.badenas@gmail.com
 *
 * This file is part of Guintftp.
 *
 * Guintftp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Guintftp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Guintftp.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <syslog.h>

#include "tftp_packet.h"

/* Devuelve el tipo de paquete del array */
int16_t guess_packet_type(char *buff, uint16_t bufflen, packet_type *type) {
	if(bufflen < 2) {
		syslog(LOG_WARNING, "Invalid buffer (guess_packet_type)!");
		return -1;
	}
	if(buff[1] < 1 || buff[1] > 5) {
		syslog(LOG_ERR, "invalid opcode");
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
		syslog(LOG_ERR, "Invalid packet type!");
		return -1;
	}
	packet->op = buff[1];
	if(bufflen > MAX_PACKET_SIZE) {
		syslog(LOG_ERR, "Buffer too long");
		return -1;
	}
	if(bufflen < MIN_READ_WRITE_SIZE) {
		syslog(LOG_ERR, "Buffer too short!");
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
		syslog(LOG_ERR, "Invalid packet!");
		return -1;
	}
	modelen = strlen(packet->mode);
	for(i = 0; i < modelen; i++) {
		packet->mode[i] = tolower(packet->mode[i]);
	}
	if((modelen != 8 && modelen != 5) || (strncmp(packet->mode, "netascii", 8) != 0 && strncmp(packet->mode, "octet", 5) != 0)) {
		syslog(LOG_ERR, "Invalid packet!");
		return -1;
	}
	return 0;
}

/* Crea un packet_data a partir del array de chars */
int16_t buff_to_packet_data(char *buff, uint16_t bufflen, packet_data **packet) {
	packet_data *pck;
	
	if(bufflen < MIN_DATA_SIZE) {
		syslog(LOG_ERR, "Buffer too short!");
		return -1;
	}
	if(buff[1] != DATA) {
		syslog(LOG_ERR, "Invalid packet type!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		syslog(LOG_ERR, "Buffer too long");
		return -1;
	}
	*packet = (packet_data*) buff;
	pck = *packet;
	pck->block = ntohs(pck->block);
	pck->op = ntohs(pck->op);
	if(pck->block <= 0) {
		syslog(LOG_ERR, "Invalid block number!");
		return -1;
	}
	return 0;
}

/* Crea un packet_ack a partir del array de chars */
int16_t buff_to_packet_ack(char *buff, uint16_t bufflen, packet_ack **packet) {
	packet_ack *pck;
	
	if(bufflen != ACK_SIZE) {
		syslog(LOG_ERR, "Incorrect buffer size!");
		return -1;
	}
	if(buff[1] != ACK) {
		syslog(LOG_ERR, "Invalid packet type!");
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
		syslog(LOG_ERR, "Buffer too short!");
		return -1;
	}
	if(bufflen > MAX_PACKET_SIZE) {
		syslog(LOG_ERR, "Buffer too long");
		return -1;
	}
	if(buff[1] != ERROR) {
		syslog(LOG_ERR, "Invalid packet type!");
		return -1;
	}
	if(buff[3] < 0 || buff[3] > 7) {
		syslog(LOG_ERR, "Invalid error code!");
		return -1;
	}
	strLen = 4;
	while(buff[strLen] != '\0' && strLen < bufflen) {
		strLen++;
	}
	if(buff[strLen] != '\0') {
		syslog(LOG_ERR, "Not null terminated string!");
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
		syslog(LOG_ERR, "invalid packet type");
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
		syslog(LOG_ERR, "invalid packet type");
		return -1;
	}
	packet->op = htons(packet->op);
	packet->block = htons(packet->block);
	*buffer = (char*) packet;
	return 4;	
}

/* Crea un array de chars a partir de un packet_error */
int16_t packet_error_to_bytes(char **buffer, packet_error *packet) {
	uint16_t len, defaultstrlen;
	char *defaultstring;
	
	if(ERROR != packet->op) {
		syslog(LOG_ERR, "invalid packet type");
		return -1;
	}
	/* Checking that error code is valid */
	if(packet->error_code > 7) {
		syslog(LOG_ERR, "Invalid error code!");
		return -1;
	}
	/* if it's not ERROR_CUSTOM then we must copy a default string for that error code */
	if(0 != packet->error_code) {
		if(error_code(packet->error_code, &defaultstring, &defaultstrlen) == -1) {
			return -1;
		}
		strcpy(packet->errmsg, defaultstring);
	}
	len = 5 + strlen(packet->errmsg);
	if(len < MIN_ERROR_SIZE) {
		syslog(LOG_ERR, "Bad error packet");
		return -1;
	}
	packet->op = htons(packet->op);
	packet->error_code = htons(packet->error_code);
	*buffer = (char*) packet;
	return len;
}

int16_t error_code(uint16_t nerror_code, char **string, uint16_t *len) {
	static char *error_codes[7] = {
	"File not found.",
	"Access violation.",
	"Disk full or allocation exceeded.",
	"Illegal TFTP operation.",
	"Unknown transfer ID.",
	"File already exists.",
	"No such user."
	};
	nerror_code--;
	if(nerror_code > 6) {
		syslog(LOG_ERR, "invalid error_code");
		return -1;
	}
	*string = error_codes[nerror_code];
	*len = strlen(error_codes[nerror_code]);
	return 0;
}
