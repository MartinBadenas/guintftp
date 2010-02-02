#ifndef TFTP_PACKET_H_
#define TFTP_PACKET_H_

enum packet_type {
	RRQ=1,
	WRQ=2,
	DATA=3,
	ACK=4,
	ERROR=5
}

enum packet_mode {
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
	// max 512 bytes
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

enum packet_type guess_packet_type(char *buff, int bufflen);

int read_packet_read_write(char *buff, int bufflen, struct packet_read_write *packet);

int read_packet_data(char *buff, int bufflen, struct packet_data *packet);

int read_packet_ack(char *buff, int bufflen, struct packet_ack *packet);

int read_packet_error(char *buff, int bufflen, struct packet_error *packet);

int write_packet_read_write(char *buffer, struct packet_read_write *packet);

int write_packet_data(char *buffer, struct packet_data *packet);

int write_packet_ack(char *buffer, struct packet_ack *packet);

int write_packet_error(char *buffer, struct packet_error *packet);

#endif /*TFTP_PACKET_H_*/
