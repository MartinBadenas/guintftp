#include <assert.h>
#include <string.h>
#include "../tftp_packet.h"

void test_guess_packet_type() {
	packet_type type;
	int r;
	char buff[] = {0, 1};
	
	r = guess_packet_type(buff, 2, &type);
	assert(r == 0);
	assert(type == RRQ);
	buff[1] = 2;
	r = guess_packet_type(buff, 2, &type);
	assert(r == 0);
	assert(type == WRQ);
	buff[1] = 3;
	r = guess_packet_type(buff, 2, &type);
	assert(r == 0);
	assert(type == DATA);
	buff[1] = 4;
	r = guess_packet_type(buff, 2, &type);
	assert(r == 0);
	assert(type == ACK);
	buff[1] = 5;
	r = guess_packet_type(buff, 2, &type);
	assert(r == 0);
	assert(type == ERROR);
	r = guess_packet_type(buff, 1, &type);
	assert(r == -1);
}

void test_buff_to_packet_read_write() {
	int len = 512+4;
	char buff[len];
	packet_read_write packet;
	char *filename = "/home/dani/file";
	char *mode = "OCTET";
	char *pnt;
	int res;
	
	// read packet
	memset(buff, 0, len*sizeof(char));
	buff[1] = 1;
	pnt = &buff[2];
	memcpy(pnt, filename, (strlen(filename) + 1)*sizeof(char));
	pnt = &buff[strlen(filename) + 1];
	memcpy(pnt, mode, (strlen(mode) + 1)*sizeof(char));
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == 0);
	assert(packet.op == 1);
	assert(strcmp(packet.filename, filename) == 0);
	assert(strcmp(packet.mode, mode) == 0);
	
	// write packet
	memset(buff, 0, len);
	buff[1] = 2;
	pnt = &buff[2];
	memcpy(pnt, filename, (strlen(filename) + 1)*sizeof(char));
	pnt = &buff[strlen(filename) + 1];
	memcpy(pnt, mode, (strlen(mode) + 1)*sizeof(char));
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == 0);
	assert(packet.op == 2);
	assert(strcmp(packet.filename, filename) == 0);
	assert(strcmp(packet.mode, mode) == 0);
	
	//wrong packet type
	buff[1] = 3;
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == -1);
	
	//wrong packet size, too short
	buff[1] = 2;
	res = buff_to_packet_read_write(buff, 5, &packet);
	assert(res == -1);
	
	//wrong packet size, too long
	buff[1] = 2;
	res = buff_to_packet_read_write(buff, 517, &packet);
	assert(res == -1);
}

void test_buff_to_packet_data() {
	int len = 4 + 512;
	char buff[len];
	packet_data packet;
	int opcode = 3;
	int block = 10; // el bloque siempre empieza por 1 no puede existir un bloque menor que 1
	char *data = "Esto llega bien";
	
	memset(buff, 0, len*sizeof(char));
	buff[1] = opcode;
	memcpy(buff, data, strlen(data) + 1);
	
}

void test_buff_to_packet_ack() {
}

void test_buff_to_packet_error() {
}

void test_packet_read_write_to_bytes() {
}

void test_packet_data_to_bytes() {
}

void test_packet_ack_to_bytes() {
}

void test_packet_error_to_bytes() {
}

void test_error_code() {
	char *error_codes[7] = {
	"File not found.",
	"Access violation.",
	"Disk full or allocation exceeded.",
	"Illegal TFTP operation.",
	"Unknown transfer ID.",
	"File already exists.",
	"No such user."
	};
	int i, len;
	char msg[255];
	for(i = 1; i < 8; i++) {
		 assert(error_code(i, msg, &len) == 0);
		 assert(strcmp(msg, error_codes[i-1]) == 0);
	}
	assert(error_code(0, msg, &len) == -1);
	assert(error_code(-1, msg, &len) == -1);
	assert(error_code(7, msg, &len) == -1);
}