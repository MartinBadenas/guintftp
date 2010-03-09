#ifndef TFTP_PACKET_H_
#define TFTP_PACKET_H_

#define MAX_PACKET_SIZE 516
#define MIN_READ_WRITE_SIZE 6
#define MIN_DATA_SIZE 4
#define MIN_ERROR_SIZE 5
#define ACK_SIZE 4

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
	unsigned short filenamelen;
	char *mode;
	unsigned short modelen;
} packet_read_write;

typedef struct {
	short op;
	unsigned short block;
	char data[512];
	unsigned short datalen;
} packet_data;

typedef struct {
	short op;
	unsigned short block;
} packet_ack;

typedef struct {
	short op;
	short error_code;
	char *errmsg;
	unsigned short errmsglen;
} packet_error;

int guess_packet_type(const char *buff, int bufflen, packet_type *type);

int buff_to_packet_read_write(const char *buff, int bufflen, packet_read_write *packet);

int buff_to_packet_data(const char *buff, int bufflen, packet_data *packet);

int buff_to_packet_ack(const char *buff, int bufflen, packet_ack *packet);

int buff_to_packet_error(const char *buff, int bufflen, packet_error *packet);

int packet_data_to_bytes(char *buffer, const packet_data *packet);

int packet_ack_to_bytes(char *buffer, const packet_ack *packet);

int packet_error_to_bytes(char *buffer, const packet_error *packet);

int error_code(int error_code, char *string, int *len);

#endif /*TFTP_PACKET_H_*/
