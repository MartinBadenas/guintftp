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

#ifndef TFTP_MODE_H_
#define TFTP_MODE_H_

#include <stdint.h>

#include "tftp_packet.h"

typedef enum {
	NETASCII=1,
	OCTET=2
} packet_mode;

/* External functions */
int16_t mode_to_chars(packet_read_write *reference, char *text, int16_t textlen);
int16_t chars_to_mode(packet_read_write *reference, char *text, int16_t textlen);
/* Internal functions */
int16_t move_right_insert(int16_t position, char charAdd, char *text);
int16_t move_right_put_after(int16_t position, char charAdd, char *text);
int16_t delete_character(int16_t position, char *text);

#endif /*TFTP_MODE_H_*/
