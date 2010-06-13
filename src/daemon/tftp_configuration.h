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

#ifndef TFTP_CONFIGURATION_H_
#define TFTP_CONFIGURATION_H_

#include <linux/limits.h>

#define MAX_USERNAME_LENGTH 32
#define MAX_CONN_STRLEN 10

typedef struct {
	char root_dir[PATH_MAX];
	char user_name[MAX_USERNAME_LENGTH];
	int max_connections;
	int seconds_timeout;
	int max_retry;
	int port;
} configuration;

int load_config(configuration *config);
int apply_config(configuration *config);
/* Internal functions */
int get_config_line(char *buff, char *conf_str, char *conf_value, size_t lenlimit);

#endif /* TFTP_CONFIGURATION_H_ */
