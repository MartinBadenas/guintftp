#include "../tftp_auxi.h"
#include <string.h>
#include <stdio.h>

void test_substr() {
}

void test_memcopy() {
	//void memcopy(void * destination, const void * source, int ini, int num);
	int dest[110];
	int src[100];
	
	memset(dest, 0, 110*sizeof(int));
	memset(src, 1, 100*sizeof(int));
	memcopy(dest, src, 9, 100*sizeof(int));
	int i;
	for(i = 0; i < 110; i++) {
		printf("dest[%d] = '%d'\n", i, dest[i]);
	}
}