#ifndef TFTP_MANAGEMENT_H_
#define TFTP_MANAGEMENT_H_

#include <netinet/in.h>

#include "tftp_mode.h"
#include "tftp_configuration.h"

typedef struct {
	int socket;
	packet_mode mode;
	struct sockaddr_in address;
	socklen_t address_len;
	struct sockaddr_in remote_address;
	socklen_t remote_address_len;
	struct sockaddr_in dummy_address;
	socklen_t dummy_address_len;
} connection;

int16_t new_connection(configuration *conf, char *packet, uint16_t len, connection *parent_conn);
void dispatch_request(char *paquet, uint16_t len, connection *parent_conn);
void send_file(connection *conn, packet_read_write *paquet);
void receive_file(connection *conn, packet_read_write *paquet);
void sig_chld();

#endif /*TFTP_MANAGEMENT_H_*/
