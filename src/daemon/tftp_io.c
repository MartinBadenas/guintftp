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

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>

#include "tftp_io.h"
#include "tftp_packet.h"

#define DEFAULT_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

int16_t close_file(int fd) {
	if(close(fd) == -1) {
		syslog(LOG_CRIT, "Failed closing file");
		return -1;
	}
	return 0;
}

int open_lseek(const char *filename, off_t desiredpos, int flags, mode_t mode) {
	int fd;
	off_t pos;
	
	if(mode == 0) {
		fd = open(filename, flags);
	} else {
		fd = open(filename, flags, mode);
	}
	if(fd == -1) {
		syslog(LOG_CRIT, "Failed opening file %s (errno: %d)", filename, errno);
		return -1;
	}
	pos = lseek(fd, desiredpos, SEEK_SET);
	if(pos == -1 || pos != desiredpos) {
		syslog(LOG_CRIT, "lseek error, filename: %s", filename);
		return -1;
	}
	return fd;
}

int16_t read_bytes(const char *filename, off_t desiredpos, char *buff, uint16_t bufflen) {
	int fd;
	ssize_t num_bytes = 0, num_readed = 0;
	
	fd = open_lseek(filename, desiredpos, O_RDONLY, 0);
	if(fd == -1) {
		return -1;
	}
	while((num_readed = read(fd, &buff[num_bytes], bufflen)) > 0) {
		bufflen -= num_readed;
		num_bytes += num_readed;
	}
	
	if(num_bytes == -1) {
		syslog(LOG_ALERT, "Couldn't read file %s", filename);
		return -1;
	}
	if(close_file(fd) == -1)
		num_bytes = -1;
	
	return num_bytes;
}
int16_t write_bytes(const char *filename, off_t desiredpos, const char *buff, uint16_t bufflen) {
	int fd, flags;
	ssize_t num_bytes = 0, error = 0, writelen;
	
	flags = O_WRONLY | O_CREAT;
	if(desiredpos == 0) {
		flags = flags | O_TRUNC;
	}
	fd = open_lseek(filename, desiredpos, flags, DEFAULT_MODE);
	if(fd == -1) {
		return -1;
	}
	writelen = bufflen;
	while(error != -1 && num_bytes != bufflen) {
		error = write(fd, &buff[num_bytes], writelen);
		num_bytes += error;
		writelen -= error;
	}
	
	if(error == -1) {
		syslog(LOG_ERR, "Couldn't write to file %s", filename);
		return -1;
	}
	
	if(close_file(fd) == -1)
		return -1;
	else
		return 0;
}
