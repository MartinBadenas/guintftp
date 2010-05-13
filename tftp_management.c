#include "tftp_management.h"
#include "tftp_packet.h"
#include "tftp_log.h"
#include "tftp_net.h"

void dispatch_request(char *packet, uint16_t len) {
	int16_t error;
	packet_type type;
	packet_read_write first_packet;
	connection conn;
	int port = 1000;
	
	error = guess_packet_type(packet, 1, &type);
	if(error == -1) {
		return;
	}
	error = buff_to_packet_read_write(packet, len, &first_packet);
	if(error == -1) {
		return;
	}
	error = open_server_conn(&conn, port);
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
	log_info("Work done, bye!");
}
void send_file(connection *conn, packet_read_write *packet) {
}
void receive_file(connection *conn, packet_read_write *packet) {
}
