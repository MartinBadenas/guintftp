#include "tftp_log.h"
#include <stdio.h>

void log_error(char *msg) {
	fprintf(stderr, "%s\n", msg);
}
void log_debug(char *msg) {
	fprintf(stderr, "%s\n", msg);
}
void log_info(char *msg) {
	fprintf(stderr, "%s\n", msg);
}
void log_warn(char *msg) {
	fprintf(stderr, "%s\n", msg);
}