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

#include "test_tftp_packet.h"
#include "test_tftp_io.h"
#include "test_tftp_mode.h"

int main_test() {
	/* test tftp packet */
	/*log_info("tftp packet start");
	test_guess_packet_type();
	test_buff_to_packet_read_write();
	test_buff_to_packet_data();
	test_buff_to_packet_ack();
	test_buff_to_packet_error();
	test_packet_data_to_bytes();
	test_packet_ack_to_bytes();
	test_packet_error_to_bytes();
	test_error_code();
	log_info("tftp packet done!");*/
	
	/* test tftp_io */
	/*log_info("tftp io start");
	test_write_block();
	test_read_block();
	log_info("tftp io done!");*/
	
	/* test tftp_mode */
	text_move_right_insert();
	text_move_right_put_after();
	test_mode_to_chars();
	test_delete_character();
	test_chars_to_mode();
	return 0;
}
