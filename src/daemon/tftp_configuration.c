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

#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <grp.h>
#include <syslog.h>
#include <fcntl.h>
#include <string.h>

#include "tftp_configuration.h"
#include "tftp_io.h"

#define DEFAULT_PORT 69
#define CONFIGURATION_FILE "/usr/local/etc/guintftp.conf"
#define MAXLINE_LEN PATH_MAX+200
#define NUMBUFF_LEN 10

int load_config(configuration *config) {
	struct servent *entry;
	int fd, error, newlineindex, i;
	ssize_t bytes_read, num_bytes;
	off_t pos;
	char buff[MAXLINE_LEN], numbuff[NUMBUFF_LEN];
	char *newline, *tmp;

	entry = getservbyname("tftp", "udp");
	if(entry == NULL) {
		config->port = DEFAULT_PORT;
	} else {
		config->port = ntohs(entry->s_port);
	}
	fd = open(CONFIGURATION_FILE, O_RDONLY);
	if(fd == -1) {
		syslog(LOG_CRIT, "Failed opening file %s", CONFIGURATION_FILE);
		return -1;
	}
	error = 0;
	num_bytes = 0;
	config->max_retry = 3;
	config->seconds_timeout = 5;
	config->root_dir[0] = '\0';
	config->user_name[0] = '\0';
	config->max_connections = 0;
	while((bytes_read = read(fd, buff, MAXLINE_LEN-1)) > 0) {
		buff[bytes_read] = '\0';
		syslog(LOG_DEBUG, "line: %s", buff);
		newline = strchr(buff, '\n');
		if(newline == NULL) {
			error = 1;
			break;
		}
		newlineindex = newline - buff;
		buff[newlineindex] = '\0';
		syslog(LOG_DEBUG, "config line: %s", buff);

		tmp = strstr(buff, "root_dir");
		if(tmp != NULL) {
			tmp = &tmp[9];
			i = 0;
			while(tmp[i] != '\n' && i < PATH_MAX) {
				config->root_dir[i] = tmp[i];
				i++;
			}
			config->root_dir[i] = '\0';
		} else {
			tmp = strstr(buff, "user_name");
			if(tmp != NULL) {
				tmp = &tmp[10];
				i = 0;
				while(tmp[i] != '\n' && i < MAX_USERNAME_LENGTH) {
					config->user_name[i] = tmp[i];
					i++;
				}
				config->user_name[i] = '\0';
			} else {
				tmp = strstr(buff, "max_connections");
				if(tmp != NULL) {
					tmp = &tmp[16];
					i = 0;
					while(tmp[i] != '\n' && i < NUMBUFF_LEN) {
						numbuff[i] = tmp[i];
						i++;
					}
					numbuff[i] = '\0';
					config->max_connections = atoi(numbuff);
				} else {
					tmp = strstr(buff, "max_retry");
					if(tmp != NULL) {
						tmp = &tmp[10];
						i = 0;
						while(tmp[i] != '\n' && i < NUMBUFF_LEN) {
							numbuff[i] = tmp[i];
							i++;
						}
						numbuff[i] = '\0';
						config->max_retry = atoi(numbuff);
					} else {
						tmp = strstr(buff, "seconds_timeout");
						if(tmp != NULL) {
							tmp = &tmp[16];
							i = 0;
							while(tmp[i] != '\n' && i < NUMBUFF_LEN) {
								numbuff[i] = tmp[i];
								i++;
							}
							numbuff[i] = '\0';
							config->seconds_timeout = atoi(numbuff);
						}
					}
				}
			}
		}

		num_bytes += newlineindex + 1;
		pos = lseek(fd, num_bytes, SEEK_SET);
		if(pos == -1 || pos != num_bytes) {
			error = 1;
			break;
		}
	}
	if(bytes_read == -1 || error) {
		error = 1;
		syslog(LOG_CRIT, "Failed reading file %s", CONFIGURATION_FILE);
	}
	if(close(fd) != -1 && !error) {
		return 0;
	} else {
		syslog(LOG_CRIT, "Failed closing file %s", CONFIGURATION_FILE);
		return -1;
	}
}

int apply_config(configuration *config) {
	struct passwd *userent = NULL;

	if(geteuid() != 0) {
		syslog(LOG_EMERG, "this process should be running as root!!");
		return -1;
	}
	if((config->user_name != NULL) && ((userent = getpwnam(config->user_name)) == NULL)) {
		syslog(LOG_EMERG, "User <%s> not found!", config->user_name);
		return -1;
	}

	if(config->root_dir == NULL || access(config->root_dir, F_OK) == -1) {
		syslog(LOG_EMERG, "Root directory not defined or not found!");
		return -1;
	}
	if(chdir(config->root_dir) == -1 || chroot(config->root_dir) == -1) {
		syslog(LOG_EMERG, "chdir or chroot failed!!");
		return -1;
	}
	if(userent != NULL) {
		if(setegid(userent->pw_gid) == -1) {
			syslog(LOG_EMERG, "setegid failed!!");
			return -1;
		}
		if(initgroups(config->user_name, userent->pw_gid) == -1) {
			syslog(LOG_EMERG, "initgroups failed!!");
			return -1;
		}
		if(seteuid(userent->pw_uid) == -1) {
			syslog(LOG_EMERG, "seteuid failed!!");
			return -1;
		}
	}
	if(config->max_connections <= 0) {
		syslog(LOG_EMERG, "max_connections can't be %d", config->max_connections);
		return -1;
	}
	if(config->max_retry < 0) {
		syslog(LOG_EMERG, "max_retry can't be %d", config->max_retry);
		return -1;
	}
	if(config->seconds_timeout <= 0) {
		syslog(LOG_EMERG, "seconds_timeout can't be %d", config->seconds_timeout);
		return -1;
	}
	return 0;
}
