#ifndef TFTP_NET_H_
#define TFTP_NET_H_

#include "tftp_management.h"

int16_t open_server_conn(connection *conn, int port);
int16_t open_client_conn(connection *conn, struct in_addr *serv_address, int port);
int16_t send_packet(connection *conn, char *packet, int len);
int16_t recv_packet(connection *conn, char *packet, int maxlen);
int16_t close_conn(connection *conn);

#endif /*TFTP_NET_H_*/