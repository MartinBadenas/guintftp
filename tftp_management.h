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
	struct sockaddr_in last_address;
	socklen_t last_address_len;
	struct sockaddr_in dummy_address;
	socklen_t dummy_address_len;
} connection;

int16_t new_connection(configuration *conf, char *packet, uint16_t len, connection *parent_conn);
int16_t dispatch_request(connection *conn, char *paquet, uint16_t len, connection *parent_conn);
int16_t send_file(connection *conn, packet_read_write *paquet);
int16_t receive_file(connection *conn, packet_read_write *paquet);
int16_t send_error(connection *conn, packet_error *error);
void sig_chld();
void wait_children_die();

#endif /*TFTP_MANAGEMENT_H_*/
