#ifndef TFTP_MANAGEMENT_H_
#define TFTP_MANAGEMENT_H_

#include <netinet/in.h>
#include "tftp_mode.h"

typedef struct {
	int socket;
	packet_mode mode;
	struct sockaddr_in address;
	unsigned int address_len;
	struct sockaddr_in remote_address;
	unsigned int remote_address_len;
} connection;

void dispatch_request(char *paquet, uint16_t len, connection *parent_conn);
void send_file(connection *conn, packet_read_write *paquet);
void receive_file(connection *conn, packet_read_write *paquet);

#endif /*TFTP_MANAGEMENT_H_*/
