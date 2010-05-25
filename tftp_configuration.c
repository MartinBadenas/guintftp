#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <grp.h>

#include "tftp_configuration.h"
#include "tftp_io.h"

#define DEFAULT_PORT 69
#define CONFIGURATION_FILE "/etc/guintftp.conf"
#define BUFFER_LEN PATH_MAX+MAX_USERNAME_LENGTH+200

int load_config(configuration *config) {
	struct servent *entry;
	int16_t i, j, buffi, len;
	char buff[BUFFER_LEN], num_buff[MAX_CONN_STRLEN];

	entry = getservbyname("tftp", "udp");
	if(entry == NULL) {
		config->port = DEFAULT_PORT;
	} else {
		config->port = ntohs(entry->s_port);
	}
	len = read_bytes(CONFIGURATION_FILE, 0, buff, BUFFER_LEN);
	if(len == -1) {
		log_error("Couldn't read configuration file!");
		return -1;
	}
	for(i = 0; i < len; i++) {
		if(buff[i] == '=') {
			if(i-8 >= 0 && buff[i-8] == 'r') {
				for(j = 0, buffi = i+1; buffi < len && buff[buffi] != '\n' && j < PATH_MAX-1; j++, buffi++) {
					config->root_dir[j] = buff[buffi];
				}
				config->root_dir[j] = '\0';
			} else if(i-9 >= 0 && buff[i-9] == 'u') {
				for(j = 0, buffi = i+1; buffi < len && buff[buffi] != '\n' && j < MAX_USERNAME_LENGTH-1; j++, buffi++) {
					config->user_name[j] = buff[buffi];
				}
				config->user_name[j] = '\0';
			} else if(i-15 >= 0 && buff[i-15] == 'm') {
				for(j = 0, buffi = i+1; buffi < len && buff[buffi] != '\n' && j < MAX_CONN_STRLEN-1; j++, buffi++) {
					num_buff[j] = buff[buffi];
				}
				num_buff[j] = '\0';
				config->max_connections = atoi(num_buff);
			}
		}
	}
	return 0;
}

int apply_config(configuration *config) {
	struct passwd *userent = NULL;

	if(geteuid() != 0) {
		log_error("this process should be running as root!!");
		return -1;
	}
	if((config->user_name != NULL) && ((userent = getpwnam(config->user_name)) == NULL)) {
		log_error("User not found!");
		return -1;
	}

	if(config->root_dir == NULL || access(config->root_dir, F_OK) == -1) {
		log_error("Root directory not defined or not found!");
		return -1;
	}
	if(chdir(config->root_dir) == -1 || chroot(config->root_dir) == -1) {
		log_error("chdir or chroot failed!!");
		return -1;
	}
	if(userent != NULL) {
		if(setegid(userent->pw_gid) == -1) {
			log_error("setegid failed!!");
			return -1;
		}
		if(initgroups(config->user_name, userent->pw_gid) == -1) {
			log_error("initgroups failed!!");
			return -1;
		}
		if(seteuid(userent->pw_uid) == -1) {
			log_error("seteuid failed!!");
			return -1;
		}
	}
	return 0;
}
