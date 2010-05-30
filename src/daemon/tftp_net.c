/*
 * Copyright (C) 2010 Dani Hernández Juárez, dhernandez0@gmail.com
 * Copyright (C) 2010 Martín Badenas Beltrán, martin.badenas@gmail.com
 *
 * This file is part of Guintftp.
 *
 * Guintftp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Guintftp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Guintftp.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <errno.h>

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
	conn->dummy_address_len = sizeof(conn->dummy_address);
	conn->last_address_len = sizeof(conn->last_address);
	memset(&conn->address, 0, conn->address_len);
	memset(&conn->remote_address, 0, conn->remote_address_len);
	memset(&conn->dummy_address, 0, conn->dummy_address_len);
	memset(&conn->last_address, 0, conn->last_address_len);
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
		syslog(LOG_ERR, "Couldn't bind connection (port %d)", port);
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
	syslog(LOG_DEBUG, "send_packet remote_address: %s", ip);
	syslog(LOG_DEBUG, "send_packet conn->socket: %d", conn->socket);
	syslog(LOG_DEBUG, "send_packet len: %d", len);
	syslog(LOG_DEBUG, "send_packet conn->remote_address_len: %d", conn->remote_address_len);
	syslog(LOG_DEBUG, "send_packet port: %d", ntohs(conn->remote_address.sin_port));
	numSent = sendto(conn->socket, packet, len, 0, (struct sockaddr *) &conn->remote_address, conn->remote_address_len);
	if(numSent == -1) {
		syslog(LOG_ERR, "Error sending data");
		return -1;
	}
	
	return 0;
}
int16_t recv_packet(connection *conn, char *packet, int maxlen) {
	ssize_t numRecv;
	
	/* Copy last address to backup if next connection is from an incorrect ip or port */
	conn->last_address_len = conn->address_len;
	conn->last_address = conn->address;

	numRecv = recvfrom(conn->socket, packet, maxlen, 0, (struct sockaddr *) &conn->remote_address, &conn->remote_address_len);
	syslog(LOG_DEBUG, "recv_packet remote_address: %s", inet_ntoa(conn->remote_address.sin_addr));
	if(numRecv == -1) {
		syslog(LOG_ERR, "Error receiving data (errno: %d)", errno);
		return -1;
	}
	if((conn->last_address_len != conn->remote_address_len || (conn->last_address.sin_addr.s_addr == conn->remote_address.sin_addr.s_addr && conn->last_address.sin_port == conn->remote_address.sin_port)) &&
		(conn->last_address_len != conn->dummy_address_len || (conn->last_address.sin_addr.s_addr == conn->dummy_address.sin_addr.s_addr && conn->last_address.sin_port == conn->dummy_address.sin_port))) {
		syslog(LOG_ERR, "Connection error, from and incorrect TID (expected: %s and was: %s)", inet_ntoa(conn->last_address.sin_addr), inet_ntoa(conn->address.sin_addr));
		return -2;
	}
	return numRecv;
}

int16_t close_conn(connection *conn) {
	return close(conn->socket);
}
