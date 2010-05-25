#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <syslog.h>

#include "tftp_management.h"
#include "tftp_packet.h"
#include "tftp_net.h"
#include "tftp_io.h"

void send_error(connection *conn, packet_error *error) {
	char *send_buff;
	int16_t send_bufflen;
	packet_error *error_tmp;

	send_bufflen = packet_error_to_bytes(&send_buff, error);
	if(send_bufflen == -1) {
		log_error("1 NOOOOO!!! HORROR!");
		exit(-1);
	}
	if(send_packet(conn, send_buff, send_bufflen) == -1) {
		log_error("2 NOOOOO!!! HORROR!");
		exit(-1);
	}
	/* Llamamos a la función buff_to_packet_ack para reordenar los bytes en el orden correcto para la arquitectura (ntohs) */
	buff_to_packet_error(send_buff, send_bufflen, &error_tmp);
}

void send_ack(connection *conn, packet_ack *ack) {
	char *send_buff;
	int16_t send_bufflen;
	packet_ack *ack_tmp;
	
	send_bufflen = packet_ack_to_bytes(&send_buff, ack);
	if(send_bufflen == -1) {
		log_error("1 NOOOOO!!! HORROR!");
		exit(-1);
	}
	if(send_packet(conn, send_buff, send_bufflen) == -1) {
		log_error("2 NOOOOO!!! HORROR!");
		exit(-1);
	}
	/* Llamamos a la función buff_to_packet_ack para reordenar los bytes en el orden correcto para la arquitectura (ntohs) */
	buff_to_packet_ack(send_buff, send_bufflen, &ack_tmp);
}

void dispatch_request(char *packet, uint16_t len, connection *parent_conn) {
	int16_t error;
	packet_type type;
	packet_read_write first_packet;
	connection conn;
	unsigned short port = ntohs(parent_conn->remote_address.sin_port);
	
	error = guess_packet_type(packet, len, &type);
	if(error == -1) {
		return;
	}
	error = buff_to_packet_read_write(packet, len, &first_packet);
	if(error == -1) {
		return;
	}
	log_info("Opening connection to client from new child process (forked)");
	error = open_client_conn(&conn, &parent_conn->remote_address, port);
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
int16_t send_file(connection *conn, packet_read_write *packet) {
}
int16_t receive_file(connection *conn, packet_read_write *first_packet) {
	packet_ack ack;
	packet_data *data;
	packet_error error;
	char recv_buff[MAX_PACKET_SIZE];
	off_t filepos;
	int16_t recv_bufflen, bytes_minus, datalen;
	
	log_info("Receiving file!!");
	if(access(first_packet->filename, F_OK) == 0) {
		syslog(LOG_ERR, "<%s> file already exists", first_packet->filename);
		error.op = ERROR;
		error.error_code = ERROR_FILE_NOT_FOUND;
		send_error(conn, &error);
		return -1;
	}
	filepos = 0;
	ack.op = ACK;
	ack.block = 0;
	send_ack(conn, &ack);
	do {
		if((recv_bufflen = recv_packet(conn, recv_buff, MAX_PACKET_SIZE)) == -1) {
			return -1;
		}
		if(recv_bufflen == -2) {
			/* TODO: The two chosen TID's are then used for the remainder of the transfer. */
		}
		if(buff_to_packet_data(recv_buff, recv_bufflen, &data) == -1) {
			return -1;
		}
		if(data->block != ack.block + 1) {
			syslog(LOG_ERR, "Packet block is not correct, should be <%d> and is <%d>", ack.block + 1, data->block);
			return -1;
		}
		bytes_minus = chars_to_mode(first_packet, data->data);
		if(bytes_minus == -1) {
			return -1;
		}
		datalen = recv_bufflen - bytes_minus - 4;
		/* datalen == 0 is only allowed as last packet */
		if(datalen == 0 && data->block == 1) {
			syslog(LOG_ERR, "datalen == 0 and block == 1, datalen == 1 is only allowed as last packet");
			return -1;
		}
		if(write_bytes(first_packet->filename, filepos, data->data, datalen) == -1) {
			return -1;
		}
		filepos += datalen;
		ack.block++;
		send_ack(conn, &ack);
	} while(recv_bufflen == MAX_PACKET_SIZE);

	return 0;
}

int num_pids = 0;

int16_t new_connection(configuration *conf, char *packet, uint16_t len, connection *parent_conn) {
	pid_t pid;

	if(num_pids == conf->max_connections) {
		log_info("max_connections reached, we're not accepting more connections");
		return -1;
	}
	pid = fork();
	if(pid == -1) {
		log_error("Couldn't fork");
		return -1;
	}
	if(pid == 0) {
		printf("forked!! remote_address: %s\n", inet_ntoa(parent_conn->remote_address.sin_addr));
		dispatch_request(packet, len, parent_conn);
		exit(0);
	} else {
		num_pids++;
	}
	return 0;
}

void sig_chld() {
	syslog(LOG_NOTICE, "Child terminated :)");
	num_pids--;
}
