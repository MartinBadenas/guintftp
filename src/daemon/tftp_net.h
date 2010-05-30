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

#include "tftp_management.h"

int16_t open_server_conn(connection *conn, unsigned short port);
int16_t open_client_conn(connection *conn, struct sockaddr_in *serv_address, unsigned short port);
int16_t send_packet(connection *conn, char *packet, int len);
int16_t recv_packet(connection *conn, char *packet, int maxlen);
int16_t close_conn(connection *conn);

#endif /*TFTP_NET_H_*/
