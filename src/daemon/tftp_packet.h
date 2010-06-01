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

#ifndef TFTP_PACKET_H_
#define TFTP_PACKET_H_

#include <stdint.h>

#define MAX_PACKET_SIZE 516
#define DATA_SIZE 512
#define MIN_READ_WRITE_SIZE 6
#define MIN_DATA_SIZE 4
#define MIN_ERROR_SIZE 5
#define ACK_SIZE 4
#define MAX_BLOCK_SIZE UINT16_MAX

#define ERROR_CUSTOM 0;
#define ERROR_FILE_NOT_FOUND 1;
#define ERROR_ACCESS_VIOLATION 2;
#define ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED 3;
#define ERROR_ILLEGAL_OPERATION 4;
#define ERROR_UNKNOWN_TRANSFER_ID 5;
#define ERROR_ALREADY_EXISTS 6;
#define ERROR_NO_SUCH_USER 7;

typedef enum {
	RRQ=1,
	WRQ=2,
	DATA=3,
	ACK=4,
	ERROR=5
} packet_type;

typedef struct {
	uint16_t op;
	char *filename;
	char *mode;
} packet_read_write;

typedef struct {
	uint16_t op;
	uint16_t block;
	char data[512];
} packet_data;

typedef struct {
	uint16_t op;
	uint16_t block;
} packet_ack;

typedef struct {
	uint16_t op;
	uint16_t error_code;
	char errmsg[512];
} packet_error;

int16_t guess_packet_type(char *buff, uint16_t bufflen, packet_type *type);

int16_t buff_to_packet_read_write(char *buff, uint16_t bufflen, packet_read_write *packet);

int16_t buff_to_packet_data(char *buff, uint16_t bufflen, packet_data **packet);

int16_t buff_to_packet_ack(char *buff, uint16_t bufflen, packet_ack **packet);

int16_t buff_to_packet_error(char *buff, uint16_t bufflen, packet_error **packet);

int16_t packet_data_to_bytes(char **buffer, packet_data *packet);

int16_t packet_ack_to_bytes(char **buffer, packet_ack *packet);

int16_t packet_error_to_bytes(char **buffer, packet_error *packet);

int16_t error_code(uint16_t nerror_code, char **string, uint16_t *len);

#endif /*TFTP_PACKET_H_*/
