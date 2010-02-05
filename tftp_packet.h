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

int buff_to_packet_read_write(char *buff, int bufflen, struct packet_read_write *packet);

int buff_to_packet_data(char *buff, int bufflen, struct packet_data *packet);

int buff_to_packet_ack(char *buff, int bufflen, struct packet_ack *packet);

int buff_to_packet_error(char *buff, int bufflen, struct packet_error *packet);

int packet_read_write_to_bytes(char *buffer, struct packet_read_write *packet);

int packet_data_to_bytes(char *buffer, struct packet_data *packet);

int packet_ack_to_bytes(char *buffer, struct packet_ack *packet);

int packet_error_to_bytes(char *buffer, struct packet_error *packet);

#endif /*TFTP_PACKET_H_*/
