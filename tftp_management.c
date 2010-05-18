#include "tftp_management.h"
#include "tftp_packet.h"
#include "tftp_log.h"
#include "tftp_net.h"

void dispatch_request(char *packet, uint16_t len, connection *parent_conn) {
	int16_t error;
	packet_type type;
	packet_read_write first_packet;
	connection conn;
	struct in_addr client;
	int port = parent_conn->address.sin_port;
	
	error = guess_packet_type(packet, len, &type);
	if(error == -1) {
		return;
	}
	error = buff_to_packet_read_write(packet, len, &first_packet);
	if(error == -1) {
		return;
	}
	client = parent_conn->address.sin_addr;
	log_info("Opening connection to client from new child procces (forked)");
	error = open_client_conn(&conn, &client, port);
	if(error == -1) {
		return;
	}
	switch(type) {
	case RRQ:
		receive_file(&conn, &first_packet);
		break;
	case WRQ:
		send_file(&conn, &first_packet);
		break;
	default:
		log_error("Not valid first packet type, not RRQ or WRQ (shouldn't reach this code)");
		return;
	}
	close_conn(&conn);
	log_info("Work done, bye!");
}
void send_file(connection *conn, packet_read_write *packet) {
}
void receive_file(connection *conn, packet_read_write *packet) {
}
