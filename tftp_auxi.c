#include "tftp_auxi.h"

void substr(char *dest, const char *orig, int ini, int num) {
	int i;
	for(i = 0; i < num; i++) {
		dest[i] = orig[i + ini];
	}
}