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
#include <syslog.h>

#include "tftp_mode.h"

int16_t mode_to_chars(packet_read_write *reference, char *text, int16_t textlen) {
/* return negative number to error.
 * return 0 to nothing changes
 * return positive number if any chars are lost*/
	int16_t i, result, tmplen;
	i = 0;
	result = 0;
	if(strncmp(reference->mode, "netascii", 8)==0) {
		while(i < textlen) {
			if(text[i] == '\n') {
				if(i == 0) {
					move_right_insert(i, '\r', text);
					result++;
				} else if(text[i - 1] != '\r') {
					move_right_insert(i, '\r', text);
					result++;
				}
			} else if(text[i] == '\r') {
				tmplen = strlen(text);
				if(i < (tmplen - 1)) {
					if(text[i + 1] != '\n') {
						move_right_put_after(i, '\n', text);
						result++;
					}
				}
			}
			i++;
		}
	}
	/* no changes, return 0 */
	return result;
}
int16_t move_right_insert(int16_t position, char charAdd, char *text) {
	int16_t max, pos;
	
	if(position < 0) {
		syslog(LOG_WARNING, "Incorrect position, not negative");
		return -1;
	}
	
	max = strlen(text);
	
	if(position >= max) {
		syslog(LOG_WARNING, "Incorrect position, to long");
		return -1;
	}
	
	pos = max - 1;
	while(pos > position) {
		text[pos] = text[pos - 1];
		pos--;
	}
	text[position] = charAdd;
	return 0;
}
int16_t move_right_put_after(int16_t position, char charAdd, char *text) {
	int16_t max, pos;
	
	if(position < 0) {
		syslog(LOG_WARNING, "Incorrect position, not negative");
		return -1;
	}
	
	max = strlen(text);
	
	if(position >= max) {
		syslog(LOG_WARNING, "Incorrect position, to long");
		return -1;
	}
	
	pos = max - 1;
	position++;
	while(pos > position) {
		text[pos] = text[pos - 1];
		pos--;
	}
	text[position] = charAdd;
	return 0;
}
int16_t chars_to_mode(packet_read_write *reference, char *text, int16_t textlen) {
/* return 0 to nothing changes
 * return negative to error
 * return positive if a change was made */
	int16_t result, i;
	result = 0;
	if(strncmp(reference->mode, "netascii", 8)==0) {
		i = 1;
		/*si encuentra \r\n o \n\r se quita la \r*/
		while(i < textlen) {
				if(text[i - 1] == '\r' && text[i] == '\n') {
					delete_character(i - 1, text, textlen);
					result++;
					textlen--;
				} else if(text[i - 1] == '\n' && text[i] == '\r') {
					delete_character(i, text, textlen);
					result++;
					textlen--;
				}
				i++;
		}
	}
	/* no changes, return 0 */
	return result;
}

int16_t delete_character(int16_t position, char *text, int16_t max) {
	if(position < 0) {
		syslog(LOG_WARNING, "Incorrect position, not negative");
		return -1;
	}
	
	if(position >= max) {
		syslog(LOG_WARNING, "Incorrect position, to long");
		return -1;
	}
	
	while(position < max) {
		text[position] = text[position + 1];
		position++;
	}
	return 0;
}
