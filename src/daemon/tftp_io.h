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

#ifndef TFTP_IO_H_
#define TFTP_IO_H_

#include <stdint.h>
#include <sys/types.h>

int16_t read_bytes(const char *filename, off_t desiredpos, char *buff, uint16_t bufflen);
int16_t write_bytes(const char *filename, off_t desiredpos, const char *buff, uint16_t bufflen);
/* Internal functions */
int16_t close_file(int fd);
int open_lseek(const char *filename, off_t desiredpos, int flags, mode_t mode);

#endif /*TFTP_IO_H_*/
