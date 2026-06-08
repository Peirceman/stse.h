#define STSE_IMPLEMENTATION
#include "../stse.h"
#include <stdio.h>

int main(void) {
	Stse_Pool pool = {0};
	int *data = stse_pool_alloc(&pool, 10*sizeof(int));
	char *buf = stse_pool_calloc(&pool, 4*1024*1024);

	for (int i = 0; i < 10; i++) {
		data[i] = i*10;
	}

	data[0] = 6;
	data[9] = 9;

	stse_pool_realloc(&pool, data, 10*sizeof(int), 12*sizeof(int));
	data[10] = 100;
	data[11] = 110;
	for (int i = 0; i < 12; i++) {
		printf("%d\n", data[i]);
	}

	buf[3*1024*1024] = 'A';
	printf("%c\n", buf[3*1024*1024]);
	stse_pool_free(&pool);
	printf("done\n");
}
