#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>

#include "tftp_net.h"

int16_t open_common(connection *conn, unsigned short port) {
	int socketfd;
	
	socketfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socketfd == -1) {
		syslog(LOG_ERR, "error opening socket");
		return -1;
	}
	conn->socket = socketfd;
	conn->address_len = sizeof(conn->address);
	conn->remote_address_len = sizeof(conn->remote_address);
	memset(&conn->address, 0, conn->address_len);
	memset(&conn->remote_address, 0, conn->remote_address_len);
	conn->address.sin_family = AF_INET;
	conn->address.sin_port = htons(port);
	return 0;
}

int16_t open_server_conn(connection *conn, unsigned short port) {
	int res;
	
	conn->address.sin_addr.s_addr = INADDR_ANY;
	res = open_common(conn, port);
	if(res == -1) {
		return -1;
	}
	if(bind(conn->socket, (struct sockaddr *)&conn->address, conn->address_len) == -1) {
		syslog(LOG_ERR, "error binding connection");
		return -1;
	}
	
	return 0;
}

int16_t open_client_conn(connection *conn, struct sockaddr_in *serv_address, unsigned short port) {
	if(open_common(conn, port) == -1) {
		return -1;
	}
	conn->remote_address = *serv_address;
	return 0;
}

int16_t send_packet(connection *conn, char *packet, int len) {
	ssize_t numSent;
	char *ip;
	
	ip = inet_ntoa(conn->remote_address.sin_addr);
	printf("send_packet remote_address: %s\n", ip);
	printf("send_packet conn->socket: %d\n", conn->socket);
	printf("send_packet len: %d\n", len);
	printf("send_packet conn->remote_address_len: %d\n", conn->remote_address_len);
	printf("send_packet port: %d", ntohs(conn->remote_address.sin_port));
	numSent = sendto(conn->socket, packet, len, 0, (struct sockaddr *) &conn->remote_address, conn->remote_address_len);
	if(numSent == -1) {
		syslog(LOG_ERR, "error sending data");
		return -1;
	}
	
	return 0;
}
int16_t recv_packet(connection *conn, char *packet, int maxlen) {
	ssize_t numRecv;
	char *ip;
	
	numRecv = recvfrom(conn->socket, packet, maxlen, 0, (struct sockaddr *) &conn->remote_address, &conn->remote_address_len);
	ip = inet_ntoa(conn->remote_address.sin_addr);
	printf("recv_packet remote_address: %s\n", ip);
	if(numRecv == -1) {
		syslog(LOG_ERR, "error receiving data");
		return -1;
	}
	return numRecv;
}

int16_t close_conn(connection *conn) {
	return close(conn->socket);
}
