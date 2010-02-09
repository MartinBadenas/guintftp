#ifndef TFTP_PACKET_H_
#define TFTP_PACKET_H_

typedef enum {
	RRQ=1,
	WRQ=2,
	DATA=3,
	ACK=4,
	ERROR=5
} packet_type;

typedef struct {
	short op;
	char *filename;
	char *mode;
} packet_read_write;

typedef struct {
	short op;
	short block;
	// max 512 bytes
	char *data;
	short datalen;
} packet_data;

typedef struct {
	short op;
	short block;
} packet_ack;

typedef struct {
	short op;
	short error_code;
	char *errMsg;
} packet_error;

int guess_packet_type(char *buff, int bufflen, packet_type *type);

int buff_to_packet_read_write(char *buff, int bufflen, packet_read_write *packet);

int buff_to_packet_data(char *buff, int bufflen, packet_data *packet);

int buff_to_packet_ack(char *buff, int bufflen, packet_ack *packet);

int buff_to_packet_error(char *buff, int bufflen, packet_error *packet);

int packet_read_write_to_bytes(char *buffer, packet_read_write *packet);

int packet_data_to_bytes(char *buffer, packet_data *packet);

int packet_ack_to_bytes(char *buffer, packet_ack *packet);

int packet_error_to_bytes(char *buffer, packet_error *packet);

#endif /*TFTP_PACKET_H_*/
