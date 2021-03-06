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

#ifndef TFTP_MANAGEMENT_H_
#define TFTP_MANAGEMENT_H_

#include <netinet/in.h>

#include "tftp_configuration.h"
#include "tftp_packet.h"
#include "tftp_net.h"

int16_t new_connection(configuration *conf, char *packet, uint16_t len, connection *parent_conn);
int16_t write_pid(void);
void sig_chld(int nsignal);
void wait_children_die(void);
/* Internal functions */
int16_t send_error(connection *conn, packet_error *error);
int16_t send_ack(connection *conn, packet_ack *ack);
int16_t send_data(connection *conn, packet_data *data, int16_t len);
int16_t dispatch_request(connection *conn, char *paquet, uint16_t len);
int16_t send_file(connection *conn, packet_read_write *first_packet);
int16_t receive_file(connection *conn, packet_read_write *first_packet);

#endif /*TFTP_MANAGEMENT_H_*/
