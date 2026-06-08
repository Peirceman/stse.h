#define STSE_BLOCK_SIZE 1024
#define STSE_IMPLEMENTATION
#include "../stse.h"
#include <stdio.h>

int main(void) {
	Stse_Pool pool = {0};
	int *data = stse_pool_alloc(&pool, 10*sizeof(int));
	char *buf = stse_pool_calloc(&pool, STSE_BLOCK_SIZE * 3 / 2);
	printf("Starting:\n");

	for (int i = 0; i < 10; i++) {
		data[i] = i*10;
	}

	data[0] = 6;
	data[9] = 9;

	data = stse_pool_realloc(&pool, data, 10*sizeof(int), 12*sizeof(int));
	data[10] = 100;
	data[11] = 110;
	for (int i = 0; i < 12; i++) {
		printf("%d\n", data[i]);
	}

	buf[STSE_BLOCK_SIZE * 5 / 4] = 'A';
	printf("%c\n", buf[STSE_BLOCK_SIZE * 5 / 4]);
	stse_pool_free(&pool);
	printf("done\n");
}
