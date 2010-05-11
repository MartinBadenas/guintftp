#ifndef TFTP_NET_H_
#define TFTP_NET_H_

#include "tftp_management.h"

int open_serv_conn(connection *conn, int port);
int send_packet(connection *conn, char *packet, int len);
int recv_packet(connection *conn, char *packet, int maxlen);
int close_conn(connection *conn);

#endif /*TFTP_NET_H_*/