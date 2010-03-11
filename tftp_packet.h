#ifndef TFTP_PACKET_H_
#define TFTP_PACKET_H_

#include <stdint.h>

#define MAX_PACKET_SIZE 516
#define MIN_READ_WRITE_SIZE 6
#define MIN_DATA_SIZE 4
#define MIN_ERROR_SIZE 5
#define ACK_SIZE 4

#define ERROR_CUSTOM 0;
#define ERROR_FILE_NOT_FOUND 1;
#define ERROR_ACCESS_VIOLATION 2;
#define ERROR_DISK_FULL 3;
#define ERROR_ILLEGAL_OPERATION 4;
#define ERROR_UNKNOWN_TRANSFER_ID 5;
#define ERROR_ALREADY_EXISTS 6;
#define ERROR_NO_SUCH_USER 7;

typedef enum {
	RRQ=1,
	WRQ=2,
	DATA=3,
	ACK=4,
	ERROR=5
} packet_type;

typedef struct {
	uint16_t op;
	char *filename;
	char *mode;
} packet_read_write;

typedef struct {
	uint16_t op;
	uint16_t block;
	char *data;
	uint16_t datalen;
} packet_data;

typedef struct {
	uint16_t op;
	uint16_t block;
} packet_ack;

typedef struct {
	uint16_t op;
	uint16_t error_code;
	char *errmsg;
} packet_error;

int guess_packet_type(char *buff, uint16_t bufflen, packet_type *type);

int buff_to_packet_read_write(char *buff, uint16_t bufflen, packet_read_write *packet);

int buff_to_packet_data(char *buff, uint16_t bufflen, packet_data *packet);

int buff_to_packet_ack(char *buff, uint16_t bufflen, packet_ack *packet);

int buff_to_packet_error(char *buff, uint16_t bufflen, packet_error *packet);

int packet_data_to_bytes(char *buffer, const packet_data *packet);

int packet_ack_to_bytes(char *buffer, const packet_ack *packet);

int packet_error_to_bytes(char *buffer, const packet_error *packet);

int error_code(uint16_t error_code, char *string, uint16_t *len);

#endif /*TFTP_PACKET_H_*/
