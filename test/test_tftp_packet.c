#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

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
	char buff[512+4];
	int len = sizeof(buff);
	packet_read_write packet;
	char filename[] = "/home/dani/file";
	char mode[] = "OCTET";
	char *pnt;
	int res;
	int i;

	/* read packet */
	memset(buff, 0, len*sizeof(char));
	buff[1] = 1;
	pnt = &buff[2];
	memcpy(pnt, filename, (strlen(filename) + 1)*sizeof(char));
	pnt = &buff[strlen(filename) + 3];
	memcpy(pnt, mode, (strlen(mode) + 1)*sizeof(char));
	
	len = 4 + strlen(filename) + 1 + strlen(mode) + 1;
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == 0);
	assert(packet.op == 1);
	assert(strcmp(packet.filename, filename) == 0);
	i=0;
	while (mode[i])
	{
		mode[i] = tolower(mode[i]);
		i++;
	}
	assert(strcmp(packet.mode, mode) == 0);
	assert(strlen(packet.filename) == strlen(filename));
	assert(strlen(packet.mode) == strlen(mode));
	
	/* write packet */
	memset(buff, 0, len);
	buff[1] = 2;
	pnt = &buff[2];
	memcpy(pnt, filename, (strlen(filename) + 1));
	pnt = &buff[strlen(filename) + 3];
	memcpy(pnt, mode, (strlen(mode) + 1));
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == 0);
	assert(packet.op == 2);
	assert(strcmp(packet.filename, filename) == 0);
	assert(strcmp(packet.mode, mode) == 0);
	assert(strlen(packet.filename) == strlen(filename));
	assert(strlen(packet.mode) == strlen(mode));
	
	/* wrong packet type */
	buff[1] = 3;
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == -1);
	
	/* wrong packet size, too short */
	buff[1] = 2;
	res = buff_to_packet_read_write(buff, 5, &packet);
	assert(res == -1);
	
	/* wrong packet size, too long */
	res = buff_to_packet_read_write(buff, 517, &packet);
	assert(res == -1);
	
	/* filename not null terminated */
	buff[2 + strlen(filename)] = -1;
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == -1);
	
	/* mode not null terminated */
	buff[2 + strlen(filename)] = '\0';
	/*buff[3 + strlen(filename) + strlen(mode)] = -1;*/
	len = 3 + strlen(filename) + strlen(mode);
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == -1);
	
	/* Reset values */
	buff[1] = 1;
	buff[3 + strlen(filename) + strlen(mode)] = '\0';
	len = 4 + strlen(filename) + strlen(mode);
	/* function check with correct value mode "octet" */
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == 0);
	/* change mode to "netascii" */
	sprintf(&buff[3+strlen(filename)], "neTaScIi");
	len = 4 + strlen(filename) + strlen("neTaScIi");
	/* function check with correct value mode "netascii" */
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == 0);
	
	/* change to incorrect mode "incorrecto" */
	sprintf(&buff[3+strlen(filename)], "incorrecto");
	len = 4 + strlen(filename) + strlen("incorrecto");
	/* function check with incorrect value mode "incorrecto" */
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == -1);
	
}

void test_buff_to_packet_data() {
	char buff[4 + 512];
	uint16_t len;
	packet_data *packet;
	short opcode = 3;
	short block = 10; /* el bloque siempre empieza por 1 no puede existir un bloque menor que 1 */
	char *data = "Esto llega bien";
	char *pnt;
	uint16_t res;
	
	len = strlen(data) + 5;
	memset(buff, 0, len*sizeof(char));
	buff[1] = (char) opcode  & 0xff;
	buff[2] = (char) block >> 8;
	buff[3] = (char) block & 0xff;
	pnt = &buff[4];
	memcpy(pnt, data, strlen(data)+1);
	
	/* buff to packet */
	res = buff_to_packet_data(buff, len, &packet);
	assert(res == 0);
	assert(packet->op == 3);
	assert(packet->block == block);
	assert(strcmp(packet->data, data) == 0);
	
	/* wrong packet type */
	buff[1] = 2;
	res = buff_to_packet_data(buff, sizeof(buff), &packet);
	assert(res == -1);
	
	/* wrong block size, to short */
	buff[1] = 3;
	block = 0;
	buff[2] = (char) (block >> 8);
	buff[3] = (char) (block & 0xff);
	res = buff_to_packet_data(buff, sizeof(buff), &packet);
	assert(res == -1);
	
}

void test_buff_to_packet_ack() {
	char buff[4];
	int len = sizeof(buff);
	packet_ack ack;
	short i;
	
	/* valid packet */
	buff[0] = 0;
	buff[1] = ACK;
	for(i = 0; i < 512; i++) {
		buff[2] = (char) i >> 8;
		buff[3] = (char) i & 0xff;
		assert(buff_to_packet_ack(buff, len, &ack) == 0);
		assert(ack.op == ACK);
		assert(ack.block == i);
	}
	
	/* invalid packet op */
	buff[1] = ACK + 1;
	assert(buff_to_packet_ack(buff, len, &ack) == -1);
	
	/* invalid packet length, too long */
	buff[1] = ACK;
	len = 5;
	assert(buff_to_packet_ack(buff, len, &ack) == -1);
	
	/* invalid packet length, too short */
	buff[1] = ACK;
	len = 3;
	assert(buff_to_packet_ack(buff, len, &ack) == -1);
}

void test_buff_to_packet_error() {
	 char buff[516];
	 char *pnt;
	 int len = 516;
	 int i, error_code;
	 packet_error error;
	 char *custom_error = "Custom error!!";
	 char *error_codes[7] = {
	"File not found.",
	"Access violation.",
	"Disk full or allocation exceeded.",
	"Illegal TFTP operation.",
	"Unknown transfer ID.",
	"File already exists.",
	"No such user."
	};
	
	/* custom error packet */
	memset(buff, 0, len);
	buff[1] = ERROR;
	buff[2] = 0;
	buff[3] = 0;
	pnt = &buff[4];
	len = strlen(custom_error);
	strncpy(pnt, custom_error, len);
	len += 4;
	assert(buff_to_packet_error(buff, len, &error) == 0);
	assert(error.op == ERROR);
	assert(error.error_code == error_code);
	assert(strcmp(custom_error, error.errmsg) == 0);
	assert(strlen(custom_error) == strlen(error.errmsg));
	
	/* valid packet */
	for(i = 0; i < 7; i++) {
		memset(buff, 0, len);
		buff[1] = ERROR;
		error_code = i + 1;
		buff[2] = (char) error_code >> 8;
		buff[3] = (char) error_code & 0xff;
		pnt = &buff[4];
		len = strlen(error_codes[i]);
		strncpy(pnt, error_codes[i], len);
		len += 4;
		assert(buff_to_packet_error(buff, len, &error) == 0);
		assert(error.op == ERROR);
		assert(error.error_code == error_code);
		assert(strcmp(error_codes[i], error.errmsg) == 0);
		assert(strlen(error_codes[i]) == strlen(error.errmsg));
	}
	
	/* TODO: invalid error_code */
	
	/* invalid opcode */
	buff[1] = ERROR + 1;
	assert(buff_to_packet_error(buff, len, &error) == -1);
	
	/* buffer too long */
	buff[1] = ERROR;
	assert(buff_to_packet_error(buff, 517, &error) == -1);
	
	/* buffer too short */
	assert(buff_to_packet_error(buff, 4, &error) == -1);
	
	/* filename not null terminated */
	buff[strlen(error_codes[i]) + 4] = -1;
	assert(buff_to_packet_error(buff, len, &error) == -1);
}

void test_packet_data_to_bytes() {
	char buff[516];
	char databuff[512];
	packet_data data;
	uint16_t len;
	short block;
	
	/* valid packet */
	data.op = DATA;
	data.block = 10;
	len = sizeof(databuff);
	memcpy(data.data, databuff, len - 4);
	assert(packet_data_to_bytes(buff, &data) == 0);
	assert(buff[1] == DATA);
	block = *(short*) &buff[2];
	assert(block == data.block);
	assert(memcmp(data.data, &buff[4], len - 4) == 0);
	
	/* invalid opcode */
	data.op = ACK;
	assert(packet_data_to_bytes(buff, &data) == -1);
	
	/* buffer too long 
	data.op = DATA;
	data.datalen = 513;
	assert(packet_data_to_bytes(buff, &data) == -1);*/
}

void test_packet_ack_to_bytes() {
	packet_ack ack;
	int result;
	char buff[4];
	int len = sizeof(buff);
	char expected[4] = {0, ACK, 0, 10};
	
	/* valid packet */
	ack.block = 10;
	ack.op = ACK;
	result = packet_ack_to_bytes(buff, &ack);
	assert(result == len);
	assert(memcmp(expected, buff, sizeof(buff)) == 0);
	
	/* invalid packet */
	ack.op = DATA;
	result = packet_ack_to_bytes(buff, &ack);
	assert(result == -1);
}

void test_packet_error_to_bytes() {
	packet_error error;
	char *string;
	char buffer[516];
	uint16_t stnglen, result, expectedlen;
	
	/* valid packet */
	error.error_code = ERROR_FILE_NOT_FOUND;
	error.op = ERROR;
	error_code(error.error_code, string, &stnglen);
	strncpy(error.errmsg, string, stnglen);
	expectedlen = 4 + stnglen;
	result = packet_error_to_bytes(buffer, &error);
	assert(result == expectedlen);
	
	/* invalid opcode */
	error.op = DATA;
	result = packet_error_to_bytes(buffer, &error);
	assert(result == -1);
	
	/* invalid error code, too long */
	error.op = ERROR;
	error.error_code = 8;
	result = packet_error_to_bytes(buffer, &error);
	assert(result == -1);
	
	/* errmsg not null terminated */
	error.error_code = ERROR_FILE_NOT_FOUND;
	error.errmsg[strlen(error.errmsg) - 1] = -1;
	result = packet_error_to_bytes(buffer, &error);
	assert(result == -1);
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
	uint16_t i, len;
	char msg[255];
	for(i = 1; i < 8; i++) {
		 assert(error_code(i, msg, &len) == 0);
		 assert(strcmp(msg, error_codes[i-1]) == 0);
	}
	assert(error_code(0, msg, &len) == -1);
	assert(error_code(-1, msg, &len) == -1);
	assert(error_code(7, msg, &len) == -1);
}