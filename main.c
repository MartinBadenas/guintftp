#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "tftp_packet.h"
#include "tftp_management.h"
#include "tftp_net.h"

#define DEFAULT_PORT	69

void wait_th() {
	wait(0);
	signal(SIGCHLD, wait_th);
}

int main(int argc, char *argv[]) {
	int16_t error, packet_len;
	connection conn;
	char first_packet[MAX_PACKET_SIZE];
	
	signal(SIGCHLD, wait_th);
	error = open_server_conn(&conn, DEFAULT_PORT);
	if(error == -1) {
		return -1;
	}
	while(1) {
		packet_len = recv_packet(&conn, first_packet, MAX_PACKET_SIZE);
		if(packet_len == -1) {
			return -1;
		}
		dispatch_request(first_packet, packet_len);
	}
	close_conn(&conn);
	return 0;
}
