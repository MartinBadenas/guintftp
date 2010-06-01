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

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <check.h>

#include "../tftp_mode.h"
#include "test_utils.h"

START_TEST(test_move_right_insert) {
	char texto[] = "prueba";
	int16_t res;

	res = move_right_insert(0, 'z', texto);
	fail_unless(res == 0);

	res = move_right_insert(1, 'y', texto);
	fail_unless(res == 0);

	res = move_right_insert(-2, 'x', texto);
	fail_unless(res == -1);

	res = move_right_insert(6, 'w', texto);
	fail_unless(res == -1);
} END_TEST

START_TEST(test_move_right_put_after) {
	char texto[] = "prueba";
	int16_t res;

	res = move_right_put_after(0, 'z', texto);
	fail_unless(res == 0);

	res = move_right_put_after(1, 'y', texto);
	fail_unless(res == 0);

	res = move_right_put_after(-2, 'x', texto);
	fail_unless(res == -1);

	res = move_right_put_after(6, 'w', texto);
	fail_unless(res == -1);
} END_TEST

START_TEST(test_mode_to_chars) {
	char textoorigen[100] = "texto que solo tiene \\n\n y que se cambia por \\r\\n";
	char textofinal[100] = "texto que solo tiene \\n\r\n y que se cambia por \\r\\";
	int16_t res;
	char mode[9] = "octet";
	packet_read_write packet;

	get_test_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen, sizeof(textoorigen));
	fail_unless(res == 0);

	sprintf(mode, "netascii");
	get_test_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen, sizeof(textoorigen));
	fail_unless(res == 1);
	fail_unless(strcmp(textoorigen, textofinal) == 0);

	sprintf(textoorigen, "texto que solo tiene \\r\r y que se cambia por \\r\\n");
	sprintf(textofinal, "texto que solo tiene \\r\r\n y que se cambia por \\r\\");
	get_test_packet_read_write(&packet, mode);
	res = mode_to_chars(&packet, textoorigen, sizeof(textoorigen));
	fail_unless(res == 1);
	fail_unless(strcmp(textoorigen, textofinal) == 0);
} END_TEST

START_TEST(test_delete_character) {
	char texto[] = "prueba";
	int16_t res, len;

	len = strlen(texto);
	res = delete_character(1, texto, len);
	fail_unless(res == 0);
	fail_unless(len == 5);

	res = delete_character(1, texto, len);
	fail_unless(res == 0);
	fail_unless(len == 4);

	res = delete_character(3, texto, len);
	fail_unless(res == 0);
	fail_unless(len == 3);

	res = delete_character(4, texto, len);
	fail_unless(res == -1);
} END_TEST

START_TEST(test_chars_to_mode) {
	char textoorigen[100] = "texto con ( \\n\\r \n\r ) , ( \\r\\n \r\n ) , ( \\n \n ) y ( \\r \r ) que pasan a ser \\n";
	char textofinal[100] = "texto con ( \\n\\r \n ) , ( \\r\\n \n ) , ( \\n \n ) y ( \\r \r ) que pasan a ser \\n";
	int16_t res;
	char mode[9] = "octet";
	packet_read_write packet;

	get_test_packet_read_write(&packet, mode);
	res = chars_to_mode(&packet, textoorigen, sizeof(textoorigen));
	fail_unless(res == 0);

	sprintf(mode, "netascii");
	get_test_packet_read_write(&packet, mode);
	res = chars_to_mode(&packet, textoorigen, sizeof(textoorigen));
	fail_unless(res == 2);
	fail_unless(strcmp(textoorigen, textofinal) == 0);
} END_TEST
