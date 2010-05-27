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

#ifndef TEST_TFTP_PACKET_H_
#define TEST_TFTP_PACKET_H_
#include "../tftp_packet.h"

void test_guess_packet_type();

void test_get_packet_read_write(packet_read_write *packet, char *mode);

void test_buff_to_packet_read_write();

void test_buff_to_packet_data();

void test_buff_to_packet_ack();

void test_buff_to_packet_error();

void test_packet_data_to_bytes();

void test_packet_ack_to_bytes();

void test_packet_error_to_bytes();

void test_error_code();

#endif /*TEST_TFTP_PACKET_H_*/
