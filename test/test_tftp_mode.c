#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../tftp_mode.h"
#include "test_tftp_packet.h"

void text_move_right_insert() {
	char texto[] = "prueba";
	int16_t res;
	res = move_right_insert(1, 'z', texto);
	assert(res == 0);
	
	res = move_right_insert(-2, 'y', texto);
	assert(res == -1);
	
	res = move_right_insert(6, 'x', texto);
	assert(res == -1);
}

void test_mode_to_chars() {
	char textoorigen[] = "texto que solo tiene \\n\n y que se cambia por \\r\\n";
	char textofinal[] = "texto que solo tiene \\n\r\n y que se cambia por \\r\\";
	int16_t res;
	char mode[] = "octet";
	packet_read_write packet;
	
	test_get_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen);
	assert(res == 0);
	
	memcpy(mode, "netascii\0", strlen("netascii\0"));
	test_get_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen);
	assert(res == -1);
	assert(strcmp(textoorigen, textofinal) == 0);
	/* TODO: las pruebas que tienen que petar */
}