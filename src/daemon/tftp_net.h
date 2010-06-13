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

#ifndef TFTP_NET_H_
#define TFTP_NET_H_

#include "tftp_configuration.h"

typedef struct {
	int socket;
	int has_timeout;
	configuration *conf;
	struct sockaddr_in address;
	socklen_t address_len;
	struct sockaddr_in remote_address;
	socklen_t remote_address_len;
	struct sockaddr_in last_address;
	socklen_t last_address_len;
	struct sockaddr_in dummy_address;
	socklen_t dummy_address_len;
} connection;

int16_t open_server_conn(connection *conn, configuration *conf, unsigned short port);
int16_t open_client_conn(connection *conn, configuration *conf, struct sockaddr_in *serv_address, unsigned short port);
int16_t send_packet(connection *conn, char *packet, int len);
int16_t recv_packet(connection *conn, char *packet, int maxlen);
int16_t close_conn(connection *conn);
/* Internal functions */
int16_t open_common(connection *conn, configuration *conf, unsigned short port);

#endif /*TFTP_NET_H_*/
