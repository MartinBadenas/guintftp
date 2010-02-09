#include "tftp_auxi.h"

void substr(char *destination, const char *source, int ini, int num) {
	memcopy(destination, source, ini, num);
	destination[ini + ini] = '\0';
}

void memcopy(void *destination, const void *source, int ini, int num) {
	int i;
	char *dst8 = (char *)destination;
	char *src8 = (char *)source;
	
	for(i = 0; i < num; i++) {
		dst8[i] = src8[i + ini];
	}
}