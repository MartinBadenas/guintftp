#ifndef TFTP_MANAGEMENT_H_
#define TFTP_MANAGEMENT_H_

struct connection {
	int socket;
	enum paquet_mode mode;
}

void dispatch_request(char *paquet);
void send_file(struct connection *conn, struct packet_read_write *paquet);
void receive_file(struct connection *conn, struct packet_read_write *paquet);

#endif /*TFTP_MANAGEMENT_H_*/
