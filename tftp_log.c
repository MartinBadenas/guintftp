#include "tftp_log.h"
#include <stdio.h>

void log_error(char *msg) {
	printf("%s\n", msg);
}
void log_debug(char *msg) {
	printf("%s\n", msg);
}
void log_info(char *msg) {
	printf("%s\n", msg);
}
void log_warn(char *msg) {
	printf("%s\n", msg);
}