#include "tftp_net.h"
#include "tftp_log.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int open_serv_conn(connection *conn, int port) {
	int socketfd;
	
	socketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketfd == -1) {
		log_error("error opening socket");
		return -1;
	}
	conn->socket = socketfd;
	conn->address_len = sizeof(conn->address);
	memset(&conn->address, 0, conn->address_len);
	conn->address.sin_family = AF_INET;
	conn->address.sin_port = htons(port);
	conn->address.sin_addr.s_addr = INADDR_ANY;
	if(bind(conn->socket, (struct sockaddr *)&conn->address, conn->address_len) == -1) {
		log_error("error binding connection");
		return -1;
	}
	
	return 0;
}
int send_packet(connection *conn, char *packet, int len) {
	ssize_t numSent;
	
	numSent = sendto(conn->socket, packet, len, 0, (struct sockaddr *) &conn->address, sizeof(conn->address_len));
	if(numSent == -1) {
		log_error("error sending data");
		return -1;
	}
	
	return 0;
}
int recv_packet(connection *conn, char *packet, int maxlen) {
	ssize_t numRecv;
	
	numRecv = recvfrom(conn->socket, packet, maxlen, 0, (struct sockaddr *) &conn->address, &conn->address_len);
	if(numRecv == -1) {
		log_error("error reveiving data");
		return -1;
	}
	return numRecv;
}

int close_conn(connection *conn) {
	return close(conn->socket);
}