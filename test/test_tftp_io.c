#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "test_tftp_io.h"
#include "../tftp_io.h"
#include "../tftp_packet.h"
#include "../tftp_log.h"

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