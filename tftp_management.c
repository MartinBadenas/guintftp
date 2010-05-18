#include <stdlib.h>
#include <stdio.h>
#include "tftp_management.h"
#include "tftp_packet.h"
#include "tftp_log.h"
#include "tftp_net.h"
#include "tftp_io.h"

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
		send_file(&conn, &first_packet);
		break;
	case WRQ:
		receive_file(&conn, &first_packet);
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
void receive_file(connection *conn, packet_read_write *first_packet) {
	packet_ack ack;
	packet_data *data;
	char *send_buff, *recv_buff;
	off_t filepos;
	int16_t send_bufflen, recv_bufflen, bytes_minus, datalen;
	
	log_info("Receiving file!! uee");
	filepos = 0;
	do {
		ack.op = ACK;
		ack.block = 0;
		send_bufflen = packet_ack_to_bytes(&send_buff, &ack);
		if(send_bufflen == -1) {
			log_error("1 NOOOOO!!! HORROR!");
			exit(-1);
		}
		if(send_packet(conn, send_buff, send_bufflen) == -1) {
			log_error("2 NOOOOO!!! HORROR!");
			exit(-1);
		}
		if(recv_packet(conn, recv_buff, recv_bufflen) == -1) {
			log_error("3 NOOOOO!!! HORROR!");
			exit(-1);
		}
		if(buff_to_packet_data(recv_buff, recv_bufflen, &data) == -1) {
			log_error("4 NOOOOO!!! HORROR!");
			exit(-1);
		}
		bytes_minus = chars_to_mode(first_packet, data->data);
		if(bytes_minus == -1) {
			log_error("5 NOOOOO!!! HORROR!");
			exit(-1);
		}
		recv_bufflen -= bytes_minus;
		datalen = recv_bufflen - 4;
		write_bytes(first_packet->filename, filepos, data->data, datalen);
		filepos += datalen;
	} while(1);
}
