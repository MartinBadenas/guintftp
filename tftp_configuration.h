#ifndef TFTP_CONFIGURATION_H_
#define TFTP_CONFIGURATION_H_

#include <limits.h>

#define MAX_USERNAME_LENGTH 32
#define MAX_CONN_STRLEN 10

typedef struct {
	char root_dir[PATH_MAX];
	char user_name[MAX_USERNAME_LENGTH];
	int max_connections;
	int port;
} configuration;

int load_config(configuration *config);
int apply_config(configuration *config);

#endif /* TFTP_CONFIGURATION_H_ */
