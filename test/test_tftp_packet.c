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
	char buff[512+4];
	int len = sizeof(buff);
	packet_read_write packet;
	char *filename = "/home/dani/file";
	char *mode = "OCTET";
	char *pnt;
	int res;
	
	/* read packet */
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
	assert(packet.filenamelen == strlen(filename));
	assert(packet.modelen == strlen(mode));
	
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
	assert(packet.filenamelen == strlen(filename));
	assert(packet.modelen == strlen(mode));
	
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
	buff[3 + strlen(filename) + strlen(mode)] = -1;
	res = buff_to_packet_read_write(buff, len, &packet);
	assert(res == -1);
}

void test_buff_to_packet_data() {
	char buff[4 + 512];
	int len = sizeof(buff);
	packet_data packet;
	short opcode = 3;
	short block = 10; /* el bloque siempre empieza por 1 no puede existir un bloque menor que 1 */
	char *data = "Esto llega bien\0";
	char *errordata = "Esto hace que pete";
	char *pnt;
	int res;
	
	memset(buff, 0, len*sizeof(char));
	buff[1] = (char) (opcode  & 0xff);
	buff[2] = (char) (block >> 8);
	buff[3] = (char) (block & 0xff);
	pnt = &buff[4];
	memcpy(pnt, data, sizeof(data));
	
	/* buff to packet */
	res = buff_to_packet_data(buff, len, &packet);
	assert(res == 0);
	assert(packet.op == 3);
	assert(packet.block == block);
	assert(strcmp(packet.data, data) == 0);
	
	/* wrong packet type */
	buff[1] = 2;
	res = buff_to_packet_data(buff, len, &packet);
	assert(res == -1);
	
	/* wrong block size, to short */
	buff[1] = 3;
	block = 0;
	buff[2] = (char) (block >> 8);
	buff[3] = (char) (block & 0xff);
	res = buff_to_packet_data(buff, len, &packet);
	assert(res == -1);
	
	/* wrong data, string data doesn't have final */
	block = 10;
	buff[2] = (char) (block >> 8);
	buff[3] = (char) (block & 0xff);
	pnt = &buff[4];
	memcpy(pnt, errordata, sizeof(errordata));
	res = buff_to_packet_data(buff, len, &packet);
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
	assert(strlen(custom_error) == error.errmsglen);
	
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
		assert(strlen(error_codes[i]) == error.errmsglen);
	}
	
	/* invalid opcode */
	buff[1] = ERROR + 1;
	assert(buff_to_packet_error(buff, len, &error) == -1);
	
	/* buffer too long */
	buff[1] = ERROR;
	len = 517;
	assert(buff_to_packet_error(buff, len, &error) == -1);
	
	/* buffer too short */
	buff[1] = ERROR;
	len = 4;
	assert(buff_to_packet_error(buff, len, &error) == -1);
	
	/* TODO: filename not null terminated */
}

void test_packet_data_to_bytes() {
	char buff[516];
	char databuff[512];
	packet_data data;
	int len;
	short block;
	
	/* valid packet */
	data.op = DATA;
	data.block = 10;
	data.datalen = sizeof(databuff);
	memcpy(data.data, databuff, data.datalen);
	len = packet_data_to_bytes(buff, &data);
	assert(len == 4 + data.datalen);
	assert(buff[1] == DATA);
	block = *(short*) &buff[2];
	assert(block == data.block);
	assert(memcmp(data.data, &buff[4], data.datalen) == 0);
	
	/* invalid opcode */
	data.op = ACK;
	assert(packet_data_to_bytes(buff, &data) == -1);
	
	/* buffer too long */
	data.op = DATA;
	data.datalen = 513;
	assert(packet_data_to_bytes(buff, &data) == -1);
	
	/* TODO: filename not null terminated */
	
	/* TODO: mode not null terminated */
	
	/* TODO: filenamelen too long */
	
	/* TODO: filenamelen too short */
	
	/* TODO: modelen too long */
	
	/* TODO: modelen too short */
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