/*
 * Copyright (C) 2010 Dani Hernández Juárez, dhernandez0@gmail.com
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

#include "tftp_block_io.h"
#include "tftp_io.h"
#include "tftp_packet.h"

void openfd(fd *file, packet_read_write *packet) {
	file->filepos = 0;
	file->is_netascii = packet->mode[0] == 'n' || packet->mode[0] == 'N';
	file->filename = packet->filename;
	file->lastwascr = 0;
}

int16_t read_block(fd *file, char *buff) {
	int16_t bytes_read = 0, i;
	size_t btocopy;

	if((bytes_read = read_bytes(file->filename, file->filepos, buff, DATA_SIZE)) == -1) {
		return -1;
	}
	file->filepos += bytes_read;
	if(file->is_netascii && bytes_read > 0) {
		if(buff[0] == '\n') {
			if(!file->lastwascr) {
				btocopy = bytes_read-2;
				if(bytes_read == DATA_SIZE) {
					btocopy--;
					file->filepos--;
				} else {
					bytes_read++;
				}
				memmove(&buff[1], &buff[0], btocopy);
				buff[0] = '\r';
			}
		}
		for(i = 1; i < bytes_read-1; i++) {
			if(buff[i] == '\n' && buff[i-1] != '\r') {
				btocopy = bytes_read-i-1;
				if(bytes_read == DATA_SIZE) {
					btocopy--;
					file->filepos--;
				} else {
					bytes_read++;
				}
				memmove(&buff[i+1], &buff[i], btocopy);
				buff[i] = '\r';
			}
		}
		if(buff[i] == '\n' && (i == 0 || buff[i-1] != '\r')) {
			buff[i] = '\r';
			if(bytes_read == DATA_SIZE) {
				file->filepos--;
			} else {
				bytes_read++;
			}
		}
		file->lastwascr = buff[i] == '\r';
	}
	return bytes_read;
}
int16_t write_block(fd *file, char *buff, int16_t bufflen) {
	int16_t error;

	if(bufflen <= 0) {
		return -1;
	}
	if(file->is_netascii) {
		error = write_netascii_block(file, buff, bufflen);
	} else {
		error = write_bytes(file->filename, file->filepos, buff, bufflen);
		if(error != -1) {
			file->filepos += bufflen;
		}
	}
	return error;
}

int16_t write_netascii_block(fd *file, char *buff, int16_t bufflen) {
	/*char tmpbuff[bufflen];*/
	char tmpbuff[DATA_SIZE];
	int16_t i, j, error;

	if(file->lastwascr && buff[0] == '\n') {
		file->filepos--;
	}
	tmpbuff[0] = buff[0];
	for(i = 1, j = 1; i < bufflen; i++) {
		if(buff[i] != '\n' || buff[i-1] != '\r') {
			tmpbuff[j] = buff[i];
			j++;
		}
	}
	if(bufflen > 0) {
		file->lastwascr = buff[i] == '\r';
	}
	error = write_bytes(file->filename, file->filepos, tmpbuff, j);
	if(error != -1) {
		file->filepos += j;
	}
	return error;
}
