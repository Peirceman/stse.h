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

	printf("part 1 done\n");

	buf = stse_pool_alloc(&pool, STSE_BLOCK_SIZE - sizeof(_Stse_Pool_Block));
	buf[STSE_BLOCK_SIZE - sizeof(_Stse_Pool_Block) - 1] = 1; // touch it
	void *a = pool.blocks;
	printf("%p\n", pool.blocks->next);
	uintptr_t save1 = stse_pool_save(pool);

	char *buf2 = stse_pool_alloc(&pool, STSE_BLOCK_SIZE/2);
	printf("\n");
	// printf("%p\n", pool.blocks);
	printf("%d\n", pool.blocks->next == a);
	printf("%p\n", pool.blocks->next->next);
	uintptr_t save2 = stse_pool_save(pool);

	buf2[STSE_BLOCK_SIZE/2-1] = 1; // set it

	char *buf3 = stse_pool_alloc(&pool, 16);
	buf3[0] = 2;
	printf("%d\n", (int) buf2[STSE_BLOCK_SIZE/2-1]);
	printf("%d\n", (int) buf2[STSE_BLOCK_SIZE/2]);

	stse_pool_rewind(&pool, save2);
	buf3 = stse_pool_alloc(&pool, 16);
	buf3[0] = 3;
	printf("%d\n", (int) buf2[STSE_BLOCK_SIZE/2-1]);
	printf("%d\n", (int) buf2[STSE_BLOCK_SIZE/2]);

	stse_pool_rewind(&pool, save1);
	printf("\n");
	printf("%d\n", pool.blocks == a);
	printf("%p\n", pool.blocks->next);

	stse_pool_free(&pool);

	printf("done\n");
}
