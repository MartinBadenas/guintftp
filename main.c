#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <signal.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include "tftp_packet.h"
#include "tftp_management.h"
#include "tftp_net.h"

#define PROGRAM_NAME "guintftp"

/* This connection (port 69 UDP) must be global because we need to close it from signal handlers */
connection conn;

void sig_term() {
	log_info("TERM signal received");
	close_conn(&conn);
	closelog();
	exit(0);
}

int main(int argc, char *argv[]) {
	int16_t error, packet_len;
	configuration config;
	char first_packet[MAX_PACKET_SIZE];
	/*
	 * LOG_PERROR - off	- we need to ensure that syslog does not write to stderr
	 * LOG_CONS - off	- we need to ensure that syslog does not write to stdout
	 * LOG_PID - on		- print PID
	 * LOG_NDELAY - on	- connect now or when syscall is called (maybe the first call is chrooted? on!)
	 * LOG_ODELAY - off	- not used, backward compatibility
	 */
	int syslog_options = LOG_PID | LOG_NDELAY;
	
	/* This created a fork and calls setsid (so the child won't die) and kills main process */
	if(daemon(0, 0) == -1) {
		/* This error cannot be logged, syslog is still not opened */
		fprintf(stderr, "daemon syscall failed (errno: %d)!\n", errno);
		return EXIT_FAILURE;
	}
	/* open syslog connection! */
	openlog(PROGRAM_NAME, syslog_options, LOG_DEBUG);
	syslog(LOG_NOTICE, "Starting...");
	syslog(LOG_NOTICE, "Setting signal handlers...");
	signal(SIGCHLD, sig_chld);
	signal(SIGTERM, sig_term);
	syslog(LOG_NOTICE, "Setting umask...");
	umask(0);
	syslog(LOG_NOTICE, "Loading configuration...");
	if(load_config(&config) == -1) {
		return EXIT_FAILURE;
	}
	error = open_server_conn(&conn, config.port);
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
		if(packet_len == -1 || new_connection(&config, first_packet, packet_len, &conn) == -1) {
			syslog(LOG_CRIT, "Coudn't create new connection!");
		}
	}
	/*
	We do this on SIGTERM handler:
	close_conn(&conn);
	*/
	return EXIT_SUCCESS;
}
