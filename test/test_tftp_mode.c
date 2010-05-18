#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../tftp_mode.h"
#include "test_tftp_packet.h"

void text_move_right_insert() {
	char texto[] = "prueba";
	int16_t res;
	
	res = move_right_insert(0, 'z', texto);
	assert(res == 0);
	
	res = move_right_insert(1, 'y', texto);
	assert(res == 0);
	
	res = move_right_insert(-2, 'x', texto);
	assert(res == -1);
	
	res = move_right_insert(6, 'w', texto);
	assert(res == -1);
}

void text_move_right_put_after() {
	char texto[] = "prueba";
	int16_t res;
	
	res = move_right_put_after(0, 'z', texto);
	assert(res == 0);
	
	res = move_right_put_after(1, 'y', texto);
	assert(res == 0);
	
	res = move_right_put_after(-2, 'x', texto);
	assert(res == -1);
	
	res = move_right_put_after(6, 'w', texto);
	assert(res == -1);
}

void test_mode_to_chars() {
	char textoorigen[100] = "texto que solo tiene \\n\n y que se cambia por \\r\\n";
	char textofinal[100] = "texto que solo tiene \\n\r\n y que se cambia por \\r\\";
	int16_t res;
	char mode[9] = "octet";
	packet_read_write packet;
	
	test_get_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen);
	assert(res == 0);
	
	sprintf(mode, "netascii");
	test_get_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen);
	assert(res == 1);
	assert(strcmp(textoorigen, textofinal) == 0);
	
	sprintf(textoorigen, "texto que solo tiene \\r\r y que se cambia por \\r\\n");
	sprintf(textofinal, "texto que solo tiene \\r\r\n y que se cambia por \\r\\");
	test_get_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen);
	assert(res == 1);
	assert(strcmp(textoorigen, textofinal) == 0);
	
	
}

void test_delete_character() {
	char texto[] = "prueba";
	int16_t res;
	
	res = delete_character(1, texto);
	assert(res == 0);
	assert(strlen(texto) == 5);
	
	res = delete_character(1, texto);
	assert(res == 0);
	assert(strlen(texto) == 4);
	
	res = delete_character(3, texto);
	assert(res == 0);
	assert(strlen(texto) == 3);
	
	res = delete_character(4, texto);
	assert(res == -1);
}

void test_chars_to_mode() {
	char textoorigen[100] = "texto con ( \\n\\r \n\r ) , ( \\r\\n \r\n ) , ( \\n \n ) y ( \\r \r ) que pasan a ser \\n";
	char textofinal[100] = "texto con ( \\n\\r \n ) , ( \\r\\n \n ) , ( \\n \n ) y ( \\r \r ) que pasan a ser \\n";
	int16_t res;
	char mode[9] = "octet";
	packet_read_write packet;
	
	test_get_packet_read_write(&packet, mode);
	res = chars_to_mode(&packet, textoorigen);
	assert(res == 0);
	
	sprintf(mode, "netascii");
	test_get_packet_read_write(&packet, mode);
	res = chars_to_mode(&packet, textoorigen);
	assert(res == 2);
	assert(strcmp(textoorigen, textofinal) == 0);
}