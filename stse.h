
#ifndef STSE_H_
#define STSE_H_

#ifndef STSE_ASSERT
#include <assert.h>
#define STSE_ASSERT assert
#endif /* STSE_ASSERT */

#ifndef STSE_FREE
#include <stdlib.h>
#define STSE_FREE free
#endif /* STSE_FREE */

#ifndef STSE_REALLOC
#include <stdlib.h>
#define STSE_REALLOC realloc
#endif /* STSE_REALLOC */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define STSE_SWAP(T, a, b) do { T temp = a; a = b; b = t; } while (0)

////////////////////
// Dynamic Arrays //
////////////////////
/* "inspired" (idea copied) from nob.h https://github.com/tsoding/nob.h
 * the `da` argument should be a pointer to struct { TYPE *items; size_t len; size_t cap }
 * to support slicing, if cap is 0 but len is set, the items pointer does not have to be
 * obtained through an allocation, any lengthening of the array wil cause a reallocation.
 * Actions that dont increase the length like removing elements dont cause a reallocation
 * and modify the original array. when you append something to a shortened slice it doesn't
 * change the original array, althoug it could, for example:
 *    da = [10, 20, 30, 40, 50]
 *    slice = stse_da_slice(da, 1, 3) // slice = [20, 30, 40], da = [10, 20, 30, 40, 50]
 *    stse_da_pop(&slice)             // slice = [20, 30], da = [10, 20, 30, 40, 50]
 *    slice->items[0] = 200           // slice = [200, 30], da = [10, 200, 30, 40, 50]
 *    stse_da_push(&slice, 100)       // slice = [200, 30, 100] (reallocated), da = [10, 200, 30, 40, 50]
 * This somewhat limits the flexibility of slices when the goal is to modify the original data, but
 * the memory management is more clear: free the slice, if a lengthening opperation is called.
 */
#ifndef STSE_DA_INITIAL_CAPACITY
#define STSE_DA_INITIAL_CAPACITY 256
#endif /* STSE_DA_INITIAL_CAPACITY */

#define stse_da_reserve(da, new_cap)                                                       \
	do {                                                                                   \
		if ((da)->cap < (new_cap)) {                                                       \
			bool sliced = false;                                                           \
			if ((da)->cap == 0) {                                                          \
				if ((da)->len != 0) sliced = true;                                         \
				(da)->cap = STSE_DA_INITIAL_CAPACITY;                                      \
			}                                                                              \
			while ((da)->cap < (new_cap)) {                                                \
				(da)->cap *= 2;                                                            \
			}                                                                              \
			if (sliced) {                                                                  \
				void *items = STSE_REALLOC(NULL, (da)->cap * sizeof(*(da)->items));        \
				memcpy(items, (da)->items, (da)->len * sizeof(*(da)->items));              \
				(da)->items = items;                                                       \
			} else {                                                                       \
				(da)->items = STSE_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items)); \
			}                                                                              \
			STSE_ASSERT((da)->items && "could not allocate memory");                       \
		}                                                                                  \
	} while (0)

#define stse_da_push(da, item)                \
	do {                                      \
		stse_da_reserve((da), (da)->len + 1); \
		(da)->items[(da)->len++] = (item);    \
	} while (0)

#define stse_da_pop(da) ((da)->items[(STSE_ASSERT((da)->len > 0), --(da)->len)])

#define stse_da_insert(da, index, item)                \
	do {                                               \
		size_t i = (index);                            \
		stse_da_reserve((da), (da)->len + 1);          \
		memmove(&(da)->items[i+1],                     \
				&(da)->items[i],                       \
				((da)->len - i)*sizeof(*(da)->items)); \
		(da)->items[i] = (item);                       \
		(da)->len++;                                   \
	} while (0)

#define stse_da_remove(da, index)                            \
	do {                                                     \
		size_t i = (index);                                  \
		STSE_ASSERT((da)->len > i && i >= 0);                \
		memmove(&(da)->items[i],                             \
				&(da)->items[i+1],                           \
				((da)->len - i - 1) * sizeof(*(da)->items)); \
		(da)->len--;                                         \
	} while (0)

#define stse_da_remove_unordered(da, index)        \
	do {                                           \
		size_t i = (index);                        \
		STSE_ASSERT((da)->len > i && i >= 0);      \
		(da)->items[i] = (da)->items[--(da)->len]; \
	} while (0)

#define stse_da_slice(T, da, start_incl, slice_length)              \
	(STSE_ASSERT((da)->len >= (start_incl) + (slice_length)), (T) { \
		.items = (da)->items + (start_incl),                        \
		.len = (slice_length),                                      \
		.cap = 0,                                                   \
	})

#define stse_da_append_all(da, new_items, count)                                    \
	do {                                                                            \
		stse_da_reserve((da), (da)->len + (count));                                 \
		memcpy((da)->items + (da)->len, (new_items), count * sizeof(*(da)->items)); \
		(da)->len += (count);                                                       \
	} while (0)

#define stse_da_free(da) STSE_FREE((da)->items)

#endif /* STSE_H_ */

#ifdef STSE_OMIT_NS
#define SWAP STSE_SWAP
#define da_reserve stse_da_reserve
#define da_push stse_da_push
#define da_pop stse_da_pop
#define da_insert stse_da_insert
#define da_remove stse_da_remove
#define da_remove_unordered stse_da_remove_unordered
#define da_slice stse_da_slice
#define da_append_all stse_da_append_all
#define da_free stse_da_free
#endif /* STSE_OMIT_NS */
