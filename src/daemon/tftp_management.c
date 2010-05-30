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

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include "tftp_management.h"
#include "tftp_packet.h"
#include "tftp_net.h"
#include "tftp_io.h"

#define PID_FILE "/var/run/guintftp.pid"

int16_t send_error(connection *conn, packet_error *error) {
	char *send_buff;
	int16_t send_bufflen;
	packet_error *error_tmp;

	send_bufflen = packet_error_to_bytes(&send_buff, error);
	if(send_bufflen == -1) {
		return -1;
	}
	if(send_packet(conn, send_buff, send_bufflen) == -1) {
		syslog(LOG_ERR, "Couldn't send error packet (error_code = %d)", error->error_code);
		return -1;
	}
	/* Llamamos a la función buff_to_packet_ack para reordenar los bytes en el orden correcto para la arquitectura (ntohs) */
	buff_to_packet_error(send_buff, send_bufflen, &error_tmp);
	return 0;
}

int16_t send_ack(connection *conn, packet_ack *ack) {
	char *send_buff;
	int16_t send_bufflen;
	packet_error error;
	packet_ack *ack_tmp;
	
	send_bufflen = packet_ack_to_bytes(&send_buff, ack);
	if(send_bufflen == -1 || send_packet(conn, send_buff, send_bufflen) == -1) {
		/* ack packet is created here, creating or sending it failed, so it's an internal error */
		syslog(LOG_CRIT, "Internal error sending ACK");
		error.op = ERROR;
		error.error_code = ERROR_CUSTOM;
		strcpy(error.errmsg, "Internal error");
		send_error(conn, &error);
		return -1;
	}
	/* Llamamos a la función buff_to_packet_ack para reordenar los bytes en el orden correcto para la arquitectura (ntohs) */
	buff_to_packet_ack(send_buff, send_bufflen, &ack_tmp);
	return 0;
}

int16_t send_data(connection *conn, packet_data *data, int16_t len) {
	char *data_buff;
	packet_error error;
	packet_data *data_tmp;

	if(packet_data_to_bytes(&data_buff, data) == -1 || send_packet(conn, data_buff, len) == -1) {
		/* data packet is created here, creating or sending it failed, so it's an internal error */
		syslog(LOG_CRIT, "Internal error sending ACK");
		error.op = ERROR;
		error.error_code = ERROR_CUSTOM;
		strcpy(error.errmsg, "Internal error");
		send_error(conn, &error);
		return -1;
	}
	/* Llamamos a la función buff_to_packet_data para reordenar los bytes en el orden correcto para la arquitectura (ntohs) */
	buff_to_packet_data(data_buff, len, &data_tmp);
	return 0;
}

int16_t dispatch_request(connection *conn, char *packet, uint16_t len, connection *parent_conn) {
	packet_type type;
	packet_error error;
	packet_read_write first_packet;
	
	error.op = ERROR;
	if(guess_packet_type(packet, len, &type) == -1) {
		error.error_code = ERROR_ILLEGAL_OPERATION;
		send_error(conn, &error);
		return -1;
	}
	if(buff_to_packet_read_write(packet, len, &first_packet) == -1) {
		error.error_code = ERROR_ILLEGAL_OPERATION;
		send_error(conn, &error);
		return -1;
	}
	switch(type) {
	case RRQ:
		if(send_file(conn, &first_packet) == -1) {
			return -1;
		}
		break;
	case WRQ:
		if(receive_file(conn, &first_packet) == -1) {
			/* Receiving file failed, remove file if it exists */
			if(access(first_packet.filename, R_OK) == 0) {
				if(unlink(first_packet.filename) == -1) {
					syslog(LOG_ERR, "Couldn't delete file after failure, remove manually: %s (errno: %d)", first_packet.filename, errno);
				} else {
					syslog(LOG_INFO, "File removed after failure: %s", first_packet.filename);
				}
			}
			return -1;
		}
		break;
	default:
		syslog(LOG_EMERG, "Not valid first packet type, not RRQ or WRQ (shouldn't reach this code)");
		return -1;
	}
	return 0;
}
int16_t send_file(connection *conn, packet_read_write *first_packet) {
	packet_error error;
	packet_ack *ack;
	packet_data data;
	int16_t recv_bufflen, read_bufflen;
	char recv_buff[ACK_SIZE];
	struct stat stat_file;
	off_t filepos = 0;

	error.op = ERROR;
	/* Check whether the file is greater than TFTP protocol can handle */
	if(stat(first_packet->filename, &stat_file) == -1) {
		switch(errno) {
		case EACCES:
		case ENAMETOOLONG:
		case ENOTDIR:
			/* Access violation */
			syslog(LOG_ALERT, "Access violation <%s>", first_packet->filename);
			error.error_code = ERROR_ACCESS_VIOLATION;
			break;
		case EIO:
		case ELOOP:
			/* No resources available! */
			syslog(LOG_ALERT, "No resources available! Error when attending request <%s>", first_packet->filename);
			error.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
			break;
		case ENOENT:
			/* If size overflows size it's too large */
			syslog(LOG_ALERT, "File is doesn't exist <%s>", first_packet->filename);
			error.error_code = ERROR_FILE_NOT_FOUND;
			break;
		case EOVERFLOW:
			/* If size overflows size it's too large */
			syslog(LOG_ALERT, "File is too large <%s>", first_packet->filename);
			error.error_code = ERROR_CUSTOM;
			strcpy(error.errmsg, "File is too large");
			break;
		}
		send_error(conn, &error);
		return -1;
	}
	/* We can't send files equal or greater than (DATA_SIZE * MAX_BLOCK_SIZE) because block field is an unsigned short,
	 * if it's exactly (DATA_SIZE * MAX_BLOCK_SIZE) we would need to send another packet without data to close the connection
	 */
	if(stat_file.st_size >= DATA_SIZE * MAX_BLOCK_SIZE) {
		error.error_code = ERROR_CUSTOM;
		strcpy(error.errmsg, "File is too large");
		send_error(conn, &error);
		return -1;
	}
	syslog(LOG_INFO, "Sending file <%s>", first_packet->filename);
	data.op = DATA;
	data.block = 1;
	do {
		/* TODO: comment */
		if((read_bufflen = read_bytes(first_packet->filename, filepos, data.data, DATA_SIZE)) == -1) {
			if(errno == ENOBUFS || errno == ENXIO || errno == ENOMEM) {
				error.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
			} else if(errno == EACCES || errno == EROFS) {
				error.error_code = ERROR_ACCESS_VIOLATION;
			} else {
				error.error_code = ERROR_CUSTOM;
				strcpy(error.errmsg, "Internal error");
			}
			send_error(conn, &error);
			return -1;
		}
		/* File may have grown during file transfer */
		if(read_bufflen == DATA_SIZE && data.block == MAX_BLOCK_SIZE) {
			syslog(LOG_ALERT, "File too large, maybe grown while sending? or a size wasn't correct?");
			error.error_code = ERROR_CUSTOM;
			strcpy(error.errmsg, "File is too large, maybe was modified during transfer");
			send_error(conn, &error);
			return -1;
		}
		/* TODO: comment */
		if(send_data(conn, &data, read_bufflen) == -1) {
			return -1;
		}
		if((recv_bufflen = recv_packet(conn, recv_buff, ACK_SIZE)) == -1) {
			if(errno == ENOBUFS || errno == ENOMEM) {
				syslog(LOG_ALERT, "Failed receiving packet (allocation exceeded, errno: %d)", errno);
				error.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
			} else {
				syslog(LOG_ERR, "Failed receiving packet");
				error.error_code = ERROR_CUSTOM;
				strcpy(error.errmsg, "Failed receiving packet");
			}
			send_error(conn, &error);
			return -1;
		}
		if(recv_bufflen == -2) {
			/* Send error BUT continue listening */
			error.error_code = ERROR_UNKNOWN_TRANSFER_ID;
			if(send_error(conn, &error) == -1) {
				return -1;
			}
			/* Let's restore correct address */
			conn->address_len = conn->last_address_len;
			conn->address = conn->last_address;
			continue;
		}
		/* Send a illegal operation if packet is not well-formed or ack block is not correct and close connection */
		if(buff_to_packet_ack(recv_buff, recv_bufflen, &ack) == -1 || ack->block != data.block) {
			error.error_code = ERROR_ILLEGAL_OPERATION;
			send_error(conn, &error);
			return -1;
		}
		/* Everything was OK, increment block and file position */
		filepos += recv_bufflen;
		data.block++;
	} while(recv_bufflen == DATA_SIZE);

	return 0;
}
int16_t receive_file(connection *conn, packet_read_write *first_packet) {
	packet_ack ack;
	packet_data *data;
	packet_error error;
	char recv_buff[MAX_PACKET_SIZE];
	int16_t recv_bufflen, bytes_minus, datalen;
	off_t filepos = 0;
	int last_packet = 0;

	error.op = ERROR;
	if(access(first_packet->filename, F_OK) == 0) {
		syslog(LOG_ERR, "File already exists <%s> ", first_packet->filename);
		error.error_code = ERROR_ALREADY_EXISTS;
		send_error(conn, &error);
		return -1;
	} else if(errno != ENOENT) {
		switch(errno) {
		case EACCES:
		case ELOOP:
		case ENAMETOOLONG:
		case ENOTDIR:
		case EROFS:
		case EFAULT:
		case ETXTBSY:
			syslog(LOG_ERR, "Access violation <%s>", first_packet->filename);
			error.error_code = ERROR_ACCESS_VIOLATION;
			break;
		case EIO:
		case ENOMEM:
			syslog(LOG_ALERT, "No resources available! Error when attending request <%s>", first_packet->filename);
			error.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
		default:
			syslog(LOG_ALERT, "Unexpected error! Error when attending request <%s>", first_packet->filename);
			error.error_code = ERROR_CUSTOM;
			strcpy(error.errmsg, "Internal error");
		}
		send_error(conn, &error);
	}
	syslog(LOG_INFO, "Receiving file <%s>", first_packet->filename);
	ack.op = ACK;
	ack.block = 0;
	if(send_ack(conn, &ack) == -1) {
		return -1;
	}
	do {
		if((recv_bufflen = recv_packet(conn, recv_buff, DATA_SIZE)) == -1) {
			if(errno == ENOBUFS || errno == ENOMEM) {
				syslog(LOG_ALERT, "Failed receiving packet (allocation exceeded, errno: %d)", errno);
				error.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
			} else {
				syslog(LOG_ERR, "Failed receiving packet");
				error.error_code = ERROR_CUSTOM;
				strcpy(error.errmsg, "Failed receiving packet");
			}
			send_error(conn, &error);
			return -1;
		}
		if(recv_bufflen == -2) {
			/* Send error BUT continue listening */
			error.error_code = ERROR_UNKNOWN_TRANSFER_ID;
			if(send_error(conn, &error) == -1) {
				return -1;
			}
			/* Let's restore correct address */
			conn->address_len = conn->last_address_len;
			conn->address = conn->last_address;
			continue;
		}
		if(buff_to_packet_data(recv_buff, recv_bufflen, &data) == -1) {
			error.error_code = ERROR_ILLEGAL_OPERATION;
			send_error(conn, &error);
			return -1;
		}
		if(data->block != ack.block + 1) {
			syslog(LOG_ERR, "Packet block is not correct, should be <%d> and is <%d>", ack.block + 1, data->block);
			error.error_code = ERROR_ILLEGAL_OPERATION;
			send_error(conn, &error);
			return -1;
		}
		datalen = recv_bufflen - 4;
		if(datalen == 0 && data->block > 1) {
			/* This is the last packet, datalen == 0 so we don't need to do anything but send ack */
			syslog(LOG_INFO, "Last packet with datalen == 0");
		} else {
			bytes_minus = chars_to_mode(first_packet, data->data, datalen);
			if(bytes_minus == -1) {
				error.error_code = ERROR_CUSTOM;
				strcpy(error.errmsg, "Bad encoded netascii");
				send_error(conn, &error);
				return -1;
			}
			/* Recalculate datalen */
			datalen -= bytes_minus;
			/* datalen == 0 is only allowed as last packet */
			if(datalen == 0 && data->block == 1) {
				syslog(LOG_ERR, "datalen == 0 and block == 1, datalen == 1 is only allowed as last packet");
				error.error_code = ERROR_CUSTOM;
				strcpy(error.errmsg, "Received an empty data packet (zero data length, and is not allowed)");
				send_error(conn, &error);
				return -1;
			}
			if(write_bytes(first_packet->filename, filepos, data->data, datalen) == -1) {
				if(errno == EFBIG || errno == ENOSPC || errno == ENOBUFS || errno == ENXIO || errno == EMFILE || errno == ENFILE || errno == ENOMEM) {
					error.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
				} else if(errno == EACCES || errno == EROFS) {
					error.error_code = ERROR_ACCESS_VIOLATION;
				} else {
					error.error_code = ERROR_CUSTOM;
					strcpy(error.errmsg, "Internal error");
				}
				send_error(conn, &error);
				return -1;
			}
			filepos += datalen;
		}
		last_packet = !(recv_bufflen == MAX_PACKET_SIZE);
		if(!last_packet && ack.block == MAX_BLOCK_SIZE - 1) {
			syslog(LOG_ERR, "Client is trying to send a file too large (exceeds TFTP block size)");
			error.error_code = ERROR_CUSTOM;
			strcpy(error.errmsg, "File is too large");
			send_error(conn, &error);
			return -1;
		}
		ack.block++;
		if(send_ack(conn, &ack) == -1) {
			return -1;
		}
	} while(!last_packet);

	return 0;
}

int num_pids = 0;

int16_t new_connection(configuration *conf, char *packet, uint16_t len, connection *parent_conn) {
	pid_t pid;
	packet_error err;
	connection conn;
	int res;
	unsigned short port = ntohs(parent_conn->remote_address.sin_port);

	err.op = ERROR;
	syslog(LOG_INFO, "Opening connection to client from new child process (forked)");
	if(open_client_conn(&conn, &parent_conn->remote_address, port) == -1) {
		/* We can't send an error packet without a connection */
		return -1;
	}
	if(num_pids == conf->max_connections) {
		syslog(LOG_ALERT, "max_connections reached, we're not accepting more connections");
		err.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
		send_error(&conn, &err);
		return -1;
	}
	pid = fork();
	if(pid == -1) {
		syslog(LOG_ALERT, "Couldn't fork");
		err.error_code = ERROR_DISK_FULL_OR_ALLOCATION_EXCEEDED;
		send_error(&conn, &err);
		return -1;
	}
	if(pid == 0) {
		syslog(LOG_DEBUG, "remote_address: %s", inet_ntoa(parent_conn->remote_address.sin_addr));
		if(dispatch_request(&conn, packet, len, parent_conn) == 0) {
			res = EXIT_SUCCESS;
		} else {
			res = EXIT_FAILURE;
		}
		close_conn(&conn);
		exit(res);
	} else {
		num_pids++;
	}
	return 0;
}

int16_t write_pid() {
	pid_t pid;
	char strpid[255];

	/* We need to write the pid to a pid file, so the init.d script can read it */
	pid = getpid();
	/* Convert pid to ascii */
	sprintf(strpid, "%d\n", pid);
	if(write_bytes(PID_FILE, 0, strpid, strlen(strpid)) == -1) {
		syslog(LOG_EMERG, "Couldn't create pid file");
		return -1;
	}
	return 0;
}

void sig_chld() {
	syslog(LOG_NOTICE, "Child terminated :)");
	num_pids--;
}

void wait_children_die() {
	int status;

	if(num_pids > 0) {
		while(num_pids != 0) {
			waitpid(-1, &status, 0);
		}
	}
}
