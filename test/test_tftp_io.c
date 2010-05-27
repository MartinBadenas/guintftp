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
#include <assert.h>
#include <stdio.h>

#include "test_tftp_io.h"
#include "../tftp_io.h"
#include "../tftp_packet.h"

void test_read_block() {
	char bytes[15360];
	int i, byteslen, bufflen;
	
	byteslen = sizeof(bytes);
	for(i = 0; i < byteslen; i++) {
		bytes[i] = i;
	}
	
	for(i = 0; i < byteslen / DATA_SIZE; i++) {
		bufflen = byteslen - i*DATA_SIZE;
		if(bufflen > DATA_SIZE) {
			bufflen = DATA_SIZE;
		}
		bufflen = read_bytes("/home/dani/workspace/guintftp/test/io_tests/write_test.bin", i*bufflen, bytes, bufflen);
		assert(bufflen != -1);
	}
	assert(i*DATA_SIZE == byteslen);
	
	assert(read_bytes("/error-non-existent-file", 100, bytes, 100) == -1);
}

void test_write_block() {
	char bytes[15360];
	int i, byteslen, bufflen;
	
	byteslen = sizeof(bytes);
	for(i = 0; i < byteslen; i++) {
		bytes[i] = i;
	}
	
	for(i = 0; i < byteslen / DATA_SIZE; i++) {
		bufflen = byteslen - i*DATA_SIZE;
		if(bufflen > DATA_SIZE) {
			bufflen = DATA_SIZE;
		}
		bufflen = write_bytes("/home/dani/workspace/guintftp/test/io_tests/write_test.bin", i*bufflen, bytes, bufflen);
		assert(bufflen != -1);
	}
	assert(i*DATA_SIZE == byteslen);
	
	assert(write_bytes("/error-non-existent-file", 100, bytes, 100) == -1);
}
