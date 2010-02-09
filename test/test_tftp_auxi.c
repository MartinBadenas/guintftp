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
	int i;
	for(i = 0; i < 100; i++) {
		printf("src[%d] = '%d'\n", i, src[i]);
	}
	memcopy(dest, src, 9*sizeof(int), 100*sizeof(int));
}