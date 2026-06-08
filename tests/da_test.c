#include "../stse.h"
#include <stdio.h>

typedef struct {
	int *items;
	size_t len;
	size_t cap;
} Ints;

void print_ints(Ints is) {
	printf("[");
	for (size_t i = 0; i < is.len; i++) {
		printf("%d", is.items[i]);
		if (i+1 < is.len) {
			printf(", ");
		}
	}
	printf("]\n");
}

int main(void) {
	Ints ints = {0};
	print_ints(ints);
	stse_da_push(&ints, 10);
	stse_da_push(&ints, 20);
	stse_da_push(&ints, 30);
	stse_da_push(&ints, 40);
	stse_da_push(&ints, 50);
	print_ints(ints);
	printf("len = %d, cap = %d\n", ints.len, ints.cap);

	int popped = stse_da_pop(&ints);
	stse_da_insert(&ints, 2, 100);
	print_ints(ints);
	printf("popped = %d\n", popped);

	stse_da_remove(&ints, 2);
	print_ints(ints);

	stse_da_remove(&ints, ints.len-1);
	print_ints(ints);

	stse_da_push(&ints, 110);
	stse_da_push(&ints, 120);
	stse_da_push(&ints, 130);
	print_ints(ints);

	stse_da_remove_unordered(&ints, 1);
	print_ints(ints);

	Ints slice = stse_da_slice(Ints, &ints, 2, ints.len - 3);
	printf("slice: ");
	print_ints(slice);
	printf("original: ");
	print_ints(ints);

	slice.items[0] = 0;

	stse_da_push(&slice, 1000);
	slice.items[1] = 1;
	printf("slice: ");
	print_ints(slice);
	printf("original: ");
	print_ints(ints);

	stse_da_append_all(&ints, slice.items, slice.len);
	print_ints(ints);

	stse_da_free(&ints);
	stse_da_free(&slice);
	printf("done\n");
}
