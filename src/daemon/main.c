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

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>

#include "tftp_packet.h"
#include "tftp_management.h"
#include "tftp_net.h"
#include "config.h"

#define DEBUG 0

void sig_term(int nsignal);

/* This connection (port 69 UDP) must be global because we need to close it from signal handlers */
connection conn;

void sig_term(int nsignal) {
	if(SIGTERM == nsignal) {
		syslog(LOG_NOTICE, "TERM signal received, closing main connection and waiting for children to die...");
		close_conn(&conn);
		wait_children_die();
		syslog(LOG_NOTICE, "Goodbye!!!");
		closelog();
		exit(EXIT_SUCCESS);
	}
}


int main(void) {
	int16_t error, packet_len;
	configuration config;
	char first_packet[MAX_PACKET_SIZE];
	struct sigaction termaction, chldaction;
	/*
	 * LOG_PERROR - off	- we need to ensure that syslog does not write to stderr
	 * LOG_CONS - off	- we need to ensure that syslog does not write to stdout
	 * LOG_PID - on		- print PID
	 * LOG_NDELAY - on	- connect now or when syscall is called (maybe the first call is chrooted? on!)
	 * LOG_ODELAY - off	- not used, backward compatibility
	 */
	int syslog_options = LOG_PID | LOG_NDELAY;

	if(geteuid() != 0) {
		fprintf(stderr, "Must run as root\n");
		return -1;
	}
	/* This created a fork and calls setsid (so the child won't die) and kills main process */
	if(daemon(0, 0) == -1) {
		/* This error cannot be logged, syslog is still not opened */
		fprintf(stderr, "daemon syscall failed (errno: %d)\n", errno);
		return EXIT_FAILURE;
	}
	/* open syslog connection! */
	openlog(PACKAGE_TARNAME, syslog_options, LOG_DAEMON);
	if(DEBUG) {
		setlogmask(LOG_UPTO(LOG_DEBUG));
	} else {
		setlogmask(LOG_UPTO(LOG_INFO));

	}
	/* Let's create pid file */
	if(write_pid() == -1) {
		return EXIT_FAILURE;
	}
	syslog(LOG_NOTICE, "Starting...");
	syslog(LOG_NOTICE, "Setting signal handlers...");
	termaction.sa_handler = sig_term;
	termaction.sa_flags = 0;
	if(sigemptyset(&termaction.sa_mask) == -1 || sigaction(SIGTERM, &termaction, NULL) == -1) {
		syslog(LOG_EMERG, "Error setting SIGTERM");
		return EXIT_FAILURE;
	}
	chldaction.sa_handler = sig_chld;
	chldaction.sa_flags = SA_NOCLDSTOP;
	if(sigemptyset(&chldaction.sa_mask) || sigaction(SIGCHLD, &chldaction, NULL) == -1) {
		syslog(LOG_EMERG, "Error setting SIGCHLD");
		return EXIT_FAILURE;
	}
	syslog(LOG_NOTICE, "Setting umask...");
	umask(0);
	syslog(LOG_NOTICE, "Loading configuration...");
	if(load_config(&config) == -1) {
		return EXIT_FAILURE;
	}
	error = open_server_conn(&conn, &config, config.port);
	if(error == -1) {
		syslog(LOG_EMERG, "Couldn't open main TFTP connection");
		return EXIT_FAILURE;
	}
	syslog(LOG_NOTICE, "Applying configuration...");
	if(apply_config(&config) == -1) {
		syslog(LOG_EMERG, "Couldn't apply configuration");
		return EXIT_FAILURE;
	}
	/* server should run forever until receives TERM signal */
	while(1) {
		syslog(LOG_NOTICE, "Waiting for connection on port <%d>...", config.port);
		packet_len = recv_packet(&conn, first_packet, MAX_PACKET_SIZE);
		if(packet_len <= 0) {
			/* This connection failed, can't send error packet because we don't know client IP:PORT, log if critical and continue! */
			if(errno == ENOBUFS || errno == ENOMEM) {
				syslog(LOG_ALERT, "Failed receiving packet (allocation exceeded, errno: %d)", errno);
			}
			continue;
		}
		new_connection(&config, first_packet, packet_len, &conn);
	}
	/* Must not reach this code, it's a failure! (EXIT_FAILURE) */
	return EXIT_FAILURE;
}
