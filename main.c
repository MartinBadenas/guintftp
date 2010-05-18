#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "tftp_log.h"
#include "tftp_packet.h"
#include "tftp_management.h"
#include "tftp_net.h"

#define DEFAULT_PORT	69

void sig_term() {
}

void sig_int() {
}

int main(int argc, char *argv[]) {
	int16_t error, packet_len;
	pid_t pid;
	connection conn;
	char first_packet[MAX_PACKET_SIZE];
	
	log_info("Starting..");
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, sig_term);
	signal(SIGINT, sig_int);
	error = open_server_conn(&conn, DEFAULT_PORT);
	if(error == -1) {
		return -1;
	}
	while(1) {
		log_info("Waiting for connection..");
		packet_len = recv_packet(&conn, first_packet, MAX_PACKET_SIZE);
		log_info("New connection!!");
		if(packet_len == -1) {
			return -1;
		}
		pid = fork();
		if(pid == -1) {
			log_error("Couldn't fork");
			return -1;
		}
		if(pid == 0) {
			dispatch_request(first_packet, packet_len, &conn);
			return 0;
		}
	}
	close_conn(&conn);
	return 0;
}
