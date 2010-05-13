#include "tftp_mode.h"
#include <string.h>
#include "tftp_log.h"

int16_t mode_to_chars(packet_read_write *reference, char *text) {
/* return negative number to error.
 * return 0 to nothing changes
 * return positive number if any chars are lose*/
	int16_t i, result;
	i = 0;
	result = 0;
	if(strncmp(reference->mode, "netascii", 8)==0) {
		while(i < strlen(text) && i < DATA_SIZE) {
			if(text[i] == '\n') {
				if(i == 0) {
					move_right_insert(i, '\r', text);
					result++;
				} else if(text[i - 1] != '\r') {
					move_right_insert(i, '\r', text);
					result++;
				}
			} else if(text[i] == '\r') {
				if(i < (strlen(text) - 1)) {
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
		log_error("Incorrect position, not negative");
		return -1;
	}
	
	max = strlen(text);
	
	if(position >= max) {
		log_error("Incorrect position, to long");
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
		log_error("Incorrect position, not negative");
		return -1;
	}
	
	max = strlen(text);
	
	if(position >= max) {
		log_error("Incorrect position, to long");
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
int16_t chars_to_mode(packet_read_write *reference, char *text) {
/* return 0 to nothing changes
 * return negative to error
 * return positive if a change was made */
	int16_t result, i;
	result = 0;
	if(strncmp(reference->mode, "netascii", 8)==0) {
		i = 1;
		/*si encuentra \r\n o \n\r se quita la \r*/
		while(i < strlen(text) && i < DATA_SIZE) {
				if(text[i - 1] == '\r' && text[i] == '\n') {
					delete_character(i - 1, text);
					result++;
				}
				else if(text[i - 1] == '\n' && text[i] == '\r') {
					delete_character(i, text);
					result++;
				}
				i++;
		}
	}
	/* no changes, return 0 */
	return result;
}

int16_t delete_character(int16_t position, char *text) {
	int16_t max;
	
	if(position < 0) {
		log_error("Incorrect position, not negative");
		return -1;
	}
	
	max = strlen(text);
	
	if(position >= max) {
		log_error("Incorrect position, to long");
		return -1;
	}
	
	while(position < max) {
		text[position] = text[position + 1];
		position++;
	}
	return 0;
}