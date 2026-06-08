#ifndef STSE_H_
#define STSE_H_

#ifndef STSEDEF
#define STSEDEF
#endif /* STSEDEF */

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


// https://github.com/tsoding/nob.h
#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    define STSE_FORMAT_STRING
#    ifdef __MINGW_PRINTF_FORMAT
#        define STSE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define STSE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif /* __MINGW_PRINTF_FORMAT */
#else
//   https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/ms235402(v=vs.100)
#    define STSE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#    if _MSC_VER >= 1400
#        include <sal.h>
#        if _MSC_VER > 1400
#            define STSE_FORMAT_STRING _Printf_format_string_
#        else
#            define STSE_FORMAT_STRING __format_string
#        endif
#    else
#        define STSE_FORMAT_STRING
#    endif
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STSE_SWAP(T, a, b) do { T temp = a; a = b; b = t; } while (0)

//////////////////////
// temporary buffer //
//////////////////////
#ifndef STSE_TMP_BUF_CAP
#define STSE_TMP_BUF_CAP (4*1024*1024)
#endif /* STSE_TMP_BUF_CAP */

STSEDEF void stse_tmp_reset(void);
STSEDEF size_t stse_tmp_save(void);
STSEDEF void stse_tmp_rewind(size_t save_state);
STSEDEF void *stse_tmp_alloc(size_t bytes);
STSEDEF char *stse_tmp_strdup(const char *src);
STSEDEF char *stse_tmp_strndup(const char *src, size_t n);
STSEDEF char *stse_tmp_sprintf(STSE_FORMAT_STRING const char *format, ...) STSE_PRINTF_FORMAT(1, 2);
STSEDEF char *stse_tmp_vsprintf(const char *format, va_list ap);

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

#define stse_da_insert(da, index, item)                    \
	do {                                                   \
		size_t i = (index);                                \
		STSE_ASSERT(i >= 0);                               \
		if (i >= (da)->len) stse_da_push((da), (item));    \
		else {                                             \
			stse_da_reserve((da), (da)->len + 1);          \
			memmove(&(da)->items[i+1],                     \
					&(da)->items[i],                       \
					((da)->len - i)*sizeof(*(da)->items)); \
			(da)->items[i] = (item);                       \
			(da)->len++;                                   \
		}                                                  \
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

#define stse_da_append_all(da, new_items, count)        \
	do {                                                \
		if ((count) > 0) {                              \
			stse_da_reserve((da), (da)->len + (count)); \
			memcpy((da)->items + (da)->len,             \
					(new_items),                        \
					(count) * sizeof(*(da)->items));    \
			(da)->len += (count);                       \
		}                                               \
	} while (0)

#define stse_da_free(da) STSE_FREE((da)->items)

//////////////////
// memory pools //
//////////////////

#ifndef STSE_BLOCK_SIZE
#define STSE_BLOCK_SIZE (4*1024)
#endif /* STSE_BLOCK_SIZE */

typedef struct _Stse_Pool_Block _Stse_Pool_Block;

struct _Stse_Pool_Block {
	_Stse_Pool_Block *next;
	size_t empty_offset;
	size_t empty_space;
	unsigned char data[];
};

typedef struct {
	_Stse_Pool_Block *blocks;
} Stse_Pool;

STSEDEF void *stse_pool_alloc(Stse_Pool *pool, size_t bytes);
STSEDEF void *stse_pool_calloc(Stse_Pool *pool, size_t bytes);
STSEDEF void *stse_pool_realloc(Stse_Pool *pool, void *data, size_t old_size, size_t new_size);
STSEDEF void stse_pool_free(Stse_Pool *pool);

#endif /* STSE_H_ */

#ifdef STSE_IMPLEMENTATION

static char stse_tmp_buffer[STSE_TMP_BUF_CAP] = {0};
static size_t stse_tmp_buffer_size = 0;

STSEDEF void stse_tmp_reset(void) {
	stse_tmp_buffer_size = 0;
}

STSEDEF size_t stse_tmp_save(void) {
	return stse_tmp_buffer_size;
}

STSEDEF void stse_tmp_rewind(size_t save_state) {
	stse_tmp_buffer_size = save_state;
}

STSEDEF void *stse_tmp_alloc(size_t bytes) {
	size_t word_size = sizeof(uintptr_t);
	bytes = (bytes+word_size-1)/word_size*word_size;
	if (stse_tmp_buffer_size + bytes > STSE_TMP_BUF_CAP) return NULL;
	void *result = &stse_tmp_buffer[stse_tmp_buffer_size];
	stse_tmp_buffer_size += bytes;
	return result;
}

STSEDEF char *stse_tmp_strdup(const char *src) {
	size_t n = strlen(src);
	char *copy = stse_tmp_alloc(n + 1);
	if (!copy) return NULL;
	memcpy(copy, src, n);
	copy[n] = '\0';
	return copy;
}

STSEDEF char *stse_tmp_strndup(const char *src, size_t n) {
	char *copy = stse_tmp_alloc(n + 1);
	if (!copy) return NULL;
	memcpy(copy, src, n);
	copy[n] = '\0';
	return copy;
}

STSEDEF char *stse_tmp_sprintf(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	char *result = stse_tmp_vsprintf(format, ap);
	va_end(ap);
	return result;
}

STSEDEF char *stse_tmp_vsprintf(const char *format, va_list ap) {
	va_list args;
	va_copy(args, ap);
	int n = vsnprintf(NULL, 0, format, args);
	va_end(args);

	STSE_ASSERT(n >= 0);

	char *result = stse_tmp_alloc(n+1);
	if (!result) return NULL;
	va_copy(args, ap);
	vsnprintf(result, n+1, format, args);
	va_end(args);

	return result;
}

STSEDEF void *stse_pool_alloc(Stse_Pool *pool, size_t bytes) {
	size_t word_size = sizeof(uintptr_t);
	bytes = (bytes + word_size - 1)/word_size*word_size;

	if (pool->blocks != NULL) {
		_Stse_Pool_Block *block = pool->blocks;
		if (block->empty_space >= bytes) {
			void *result = block->data + block->empty_offset;
			block->empty_offset += bytes;
			block->empty_space -= bytes;
			return result;
		}
	}

	size_t n = STSE_BLOCK_SIZE;
	while (bytes + sizeof(_Stse_Pool_Block) > n) n *= 2;

	_Stse_Pool_Block *block = STSE_REALLOC(NULL, n);
	if (!block) return NULL;

	*block = (_Stse_Pool_Block) {
		.next = pool->blocks,
		.empty_offset = bytes,
		.empty_space = n - sizeof(_Stse_Pool_Block) - bytes,
	};

	pool->blocks = block;

	return block->data;
}

STSEDEF void *stse_pool_calloc(Stse_Pool *pool, size_t bytes) {
	size_t word_size = sizeof(uintptr_t);
	bytes = (bytes + word_size - 1)/word_size*word_size;
	void *result = stse_pool_alloc(pool, bytes);
	memset(result, 0, bytes);
	return result;
}

STSEDEF void *stse_pool_realloc(Stse_Pool *pool, void *data, size_t old_size, size_t new_size) {
	size_t word_size = sizeof(uintptr_t);
	new_size = (new_size + word_size - 1)/word_size*word_size;
	void *result = stse_pool_alloc(pool, new_size);
	memcpy(result, data, old_size);
	return result;
}

STSEDEF void stse_pool_free(Stse_Pool *pool) {
	_Stse_Pool_Block *block = pool->blocks;
	while (block) {
		_Stse_Pool_Block *next = block->next;
		STSE_FREE(block);
		block = next;
	}

	pool->blocks = NULL;
}

#endif /* STSE_IMPLEMENTATION */

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
#define tmp_reset stse_tmp_reset
#define tmp_save stse_tmp_save
#define tmp_rewind stse_tmp_rewind
#define tmp_alloc stse_tmp_alloc
#define tmp_sprintf stse_tmp_sprintf
#endif /* STSE_OMIT_NS */
