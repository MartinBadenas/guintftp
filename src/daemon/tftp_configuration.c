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
	int fd, error, newlineindex;
	ssize_t bytes_read, num_bytes;
	off_t pos;
	char buff[MAXLINE_LEN], numbuff[NUMBUFF_LEN];
	char *newline;

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

		if(!get_config_line(buff, "root_dir", config->root_dir, PATH_MAX)) {
			if(!get_config_line(buff, "user_name", config->user_name, MAX_USERNAME_LENGTH)) {
				if(get_config_line(buff, "max_connections", numbuff, NUMBUFF_LEN)) {
					config->max_connections = atoi(numbuff);
				} else if(get_config_line(buff, "max_retry", numbuff, NUMBUFF_LEN)) {
					config->max_retry = atoi(numbuff);
				} else if(get_config_line(buff, "seconds_timeout", numbuff, NUMBUFF_LEN)) {
					config->seconds_timeout = atoi(numbuff);
				} else {
					syslog(LOG_DEBUG, "<%s> line didn't have any config!", buff);
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

int get_config_line(char *buff, char *conf_str, char *conf_value, size_t lenlimit) {
	size_t conflen, linelen, i, lim;
	char *tmp;

	tmp = strstr(buff, conf_str);
	if(tmp != NULL) {
		linelen = strlen(tmp);
		conflen = strlen(conf_str);
		if(linelen > conflen) {
			tmp = &tmp[conflen];
			lim = linelen - conflen;
			i = 0;
			while(tmp[i] != '\n' && i < lenlimit && i < lim) {
				conf_value[i] = tmp[i];
				i++;
			}
			conf_value[i] = '\0';
		}
		return 1;
	}
	return 0;
}

int apply_config(configuration *config) {
	struct passwd *userent = NULL;

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
