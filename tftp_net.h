#ifndef TFTP_NET_H_
#define TFTP_NET_H_

void open_conn(struct connection *conn, int port);
void send_packet(struct connection *conn, char *packet);
void recv_packet(struct connection *conn, char *packet);
void close_conn(struct connection *conn);

#endif /*TFTP_NET_H_*/