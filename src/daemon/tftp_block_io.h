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

#ifndef TFTP_BLOCK_IO_H_
#define TFTP_BLOCK_IO_H_

#include <stdint.h>
#include <sys/types.h>

#include "tftp_packet.h"

typedef struct {
	char *filename;
	off_t filepos;
	int is_netascii;
	int lastwascr;
	int nextblockinsertlf;
} fd;

void openfd(fd *file, packet_read_write *packet);
int16_t read_block(fd *file, char *buff);
int16_t write_block(fd *file, char *buff, int16_t bufflen);
int16_t write_netascii_block(fd *file, char *buff, int16_t bufflen);

#endif /* TFTP_BLOCK_IO_H_ */
