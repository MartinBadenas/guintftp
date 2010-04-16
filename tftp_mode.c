#include "tftp_mode.h"
#include <string.h>
#include "tftp_log.h"

int16_t mode_to_chars(packet_read_write *reference, char *text) {
/* return negative number to characters lose.
 * return 0 to nothing changes
 * return positive number to complet de 512 bytes of data*/
	int16_t i, result;
	i = 0;
	result = 0;
	if(strncmp(reference->mode, "netascii", 8)==0) {
		while(i < strlen(text) && i < DATA_SIZE) {
			if(text[i] == '\n') {
				if(i == 0) {
					move_right_insert(i, '\r', text);
					result--;
				} else if(text[i - 1] != '\r') {
					move_right_insert(i, '\r', text);
					result--;
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
int16_t chars_to_mode(packet_read_write *reference, char *text) {
	if(strncmp(reference->mode, "netascii", 8)==0) {
		/*si encuentra \r\n o \n\r se quita la \r*/
	}
	/* no changes, return 0 */
	return 0;
}