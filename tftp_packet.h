#ifndef TFTP_PACKET_H_
#define TFTP_PACKET_H_

enum paquet_type {
	RRQ=1,
	WRQ=2,
	DATA=3,
	ACK=4,
	ERROR=5
}

enum paquet_mode {
	NETASCII="NETASCII",
	OCTET="OCTET"
}

struct packet_read_write {
	short op;
	char *filename;
	char *mode;
}

struct packet_data {
	short op;
	short block;
	char *data;
}

struct packet_ack {
	short op;
	short block;
}

struct packet_error {
	short op;
	short error_code;
	char *errMsg;
}

enum paquet_type guess_paquet_type(char *buffer);

void read_packet_read_write(char *buffer, struct packet_read_write *paquet);

void read_packet_data(char *buffer, struct packet_data *paquet);

void read_packet_ack(char *buffer, struct packet_ack *paquet);

void read_packet_error(char *buffer, struct packet_error *paquet);

void write_packet_read_write(char *buffer, struct packet_read_write *paquet);

void write_packet_data(char *buffer, struct packet_data *paquet);

void write_packet_ack(char *buffer, struct packet_ack *paquet);

void write_packet_error(char *buffer, struct packet_error *paquet);

#endif /*TFTP_PACKET_H_*/
