#include <string.h>
#include <assert.h>

#include "test_tftp_io.h"
#include "../tftp_io.h"
#include "../tftp_packet.h"

void test_read_block() {
	char bytes[2600];
	char buff[DATA_SIZE];
	int i, byteslen, bufflen;
	
	byteslen = sizeof(bytes);
	for(i = 0; i < byteslen; i++) {
		bytes[i] = i;
	}
	
	for(i = 0; i < byteslen / DATA_SIZE; i++) {
		bufflen = read_block("./io_tests/write_test.bin", i+1, buff);
		assert(memcmp(buff, &bytes[i*DATA_SIZE], bufflen) == 0);
	}
	assert(i*DATA_SIZE + bufflen == byteslen);
}

void test_write_block() {
	char buff[DATA_SIZE];
	char bytes[2600];
	int i, byteslen, bufflen;
	
	byteslen = sizeof(bytes);
	for(i = 0; i < byteslen; i++) {
		bytes[i] = i;
	}
	
	for(i = 0; i < byteslen / DATA_SIZE; i++) {
		bufflen = byteslen - (i*DATA_SIZE);
		memcpy(buff, &bytes[i*DATA_SIZE], bufflen);
		assert(write_block("./io_tests/write_test.bin", i+1, buff, bufflen) == 0);
	}
}