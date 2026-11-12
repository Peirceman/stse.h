/* The MIT License (MIT)
 *
 * Copyright © 2026 Peirceman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * library of stuf that should exist in c
 * macros to edit behaviour
 *     - `STSEDEF`: gets put in front of every function definition eg. static inline, default: ``
 *     - `STSE_REALLOC`: which function to use for realloc calls, default: `realloc`
 *     - `STSE_FREE`: which function to use for free calls, default: `free`
 *     - `STSE_ASSERT`: which function to use for assert calls, default: `assert`
 *     - `STSE_TMP_BUF_CAP`: capacity (in bytes) for the temporary buffer, default: 4mb
 *     - `STSE_BLOCK_SIZE`: default size (in bytes) for memory blocks used in memory pools, default: 256kb
 *     - `STSE_DA_INITIAL_CAPACITY`: initial capacity (in elements) for dynamic arrays, default: 256
 *     - `STSE_OMIT_NS`: omits `stse` in front of functions, default: not set
 *     - `STSE_IMPLEMENTATION`: this library in in the stile of stb header-only libraries. The header file
 *           both defines and implements all functions. If you define this macro, the implementations
 *           are included in the current source file. Usually this should only be done in one file, unless
 *           you know what you're doing (eg. when `STSEDEF` is set to `static`), default: not set
 *     - `STSE_NO_ASSERT_ALLOCATION`: when unset, assert all memory allocations are valid (not NULL).
 *           If set, return NULL in functions that deal with allocations (other functions in stse may
 *           still assert allocations are succesfull, even when this is set), default: unset
 *     - `STSE_TMP_POOL`: when set, use the defined `Stse_Pool` for the tmp buffer instead of a static
 *           array. If this value is set, `STSE_TMP_BUF_CAP` has no effect. default: unset
 *
 * LICENSE of nob.h <github.com/tsoding/nob.h> where a lot of inspiration was taken
 * and code was copied from:
 * --------------------------------------------------------------------------------
 * Copyright (c) 2024 Alexey Kutepov
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * --------------------------------------------------------------------------------
 */
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

#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// usefull macros
#define STSE_ARRAY_LEN(arr) (sizeof((arr))/sizeof(*(arr)))
#define STSE_SWAP(T, a, b) do { T temp = a; a = b; b = t; } while (0)
#define STSE_SHIFT(arr, arr_len) (STSE_ASSERT((arr_len) > 0), (arr_len)--, *(arr)++)

//////////////////////
// temporary buffer //
//////////////////////
#ifndef STSE_TMP_BUF_CAP
#define STSE_TMP_BUF_CAP (4*1024*1024)
#endif /* STSE_TMP_BUF_CAP */

STSEDEF void stse_tmp_reset(void);
STSEDEF uintptr_t stse_tmp_save(void);
STSEDEF void stse_tmp_rewind(uintptr_t save_state);
STSEDEF void *stse_tmp_alloc(size_t bytes);
STSEDEF void *stse_tmp_calloc(size_t bytes);
STSEDEF void *stse_tmp_realloc(void *data, size_t old_size, size_t new_size);
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

#if STSE_NO_ASSERT_ALLOCATION
#define STSE_ALLOC_ASSERT(x) (void)0 // probably messes up some registers but who cares.
#else
#define STSE_ALLOC_ASSERT(x) STSE_ASSERT(x)
#endif

#define stse_da_reserve(da, new_cap)                                                                  \
	do {                                                                                              \
		if ((da)->cap < (new_cap)) {                                                                  \
			bool __stse_da_reserve_sliced = false;                                                    \
			if ((da)->cap == 0) {                                                                     \
				if ((da)->len != 0) __stse_da_reserve_sliced = true;                                  \
				(da)->cap = STSE_DA_INITIAL_CAPACITY;                                                 \
			}                                                                                         \
			while ((da)->cap < (new_cap)) {                                                           \
				(da)->cap *= 2;                                                                       \
			}                                                                                         \
			if (__stse_da_reserve_sliced) {                                                           \
				void *__stse_da_reserve_items = STSE_REALLOC(NULL, (da)->cap * sizeof(*(da)->items)); \
				STSE_ALLOC_ASSERT((da)->items && "could not allocate memory");                        \
				memcpy(__stse_da_reserve_items, (da)->items, (da)->len * sizeof(*(da)->items));       \
				(da)->items = __stse_da_reserve_items;                                                \
			} else {                                                                                  \
				(da)->items = STSE_REALLOC((da)->items, (da)->cap * sizeof(*(da)->items));            \
				STSE_ALLOC_ASSERT((da)->items && "could not allocate memory");                        \
			}                                                                                         \
		}                                                                                             \
	} while (0)

#define stse_da_push(da, item)                \
	do {                                      \
		stse_da_reserve((da), (da)->len + 1); \
		(da)->items[(da)->len++] = (item);    \
	} while (0)

#define stse_da_pop(da) ((da)->items[(STSE_ASSERT((da)->len > 0), --(da)->len)])

#define stse_da_insert(da, index, item)                                     \
	do {                                                                    \
		size_t __stse_da_insert_i = (index);                                \
		STSE_ASSERT(__stse_da_insert_i >= 0);                               \
		if (__stse_da_insert_i >= (da)->len) stse_da_push((da), (item));    \
		else {                                                              \
			stse_da_reserve((da), (da)->len + 1);                           \
			memmove(&(da)->items[__stse_da_insert_i+1],                     \
					&(da)->items[__stse_da_insert_i],                       \
					((da)->len - __stse_da_insert_i)*sizeof(*(da)->items)); \
			(da)->items[__stse_da_insert_i] = (item);                       \
			(da)->len++;                                                    \
		}                                                                   \
	} while (0)

#define stse_da_remove(da, index)                                               \
	do {                                                                        \
		size_t __stse_da_remove_i = (index);                                    \
		STSE_ASSERT((da)->len > __stse_da_remove_i && __stse_da_remove_i >= 0); \
		memmove(&(da)->items[__stse_da_remove_i],                               \
				&(da)->items[__stse_da_remove_i+1],                             \
				((da)->len - __stse_da_remove_i - 1) * sizeof(*(da)->items));   \
		(da)->len--;                                                            \
	} while (0)

#define stse_da_remove_unordered(da, index)                                                         \
	do {                                                                                            \
		size_t __stse_da_remove_unordered_i = (index);                                              \
		STSE_ASSERT((da)->len > __stse_da_remove_unordered_i && __stse_da_remove_unordered_i >= 0); \
		(da)->items[__stse_da_remove_unordered_i] = (da)->items[--(da)->len];                       \
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

#define stse_da_tmp_reserve(da, new_cap)                                                              \
	do {                                                                                              \
		if ((da)->cap < (new_cap)) {                                                                  \
			bool __stse_da_tmp_reserve_sliced = false;                                                \
			if ((da)->cap == 0) {                                                                     \
				if ((da)->len != 0) __stse_da_tmp_reserve_sliced = true;                              \
				(da)->cap = STSE_DA_INITIAL_CAPACITY;                                                 \
			}                                                                                         \
			while ((da)->cap < (new_cap)) {                                                           \
				(da)->cap *= 2;                                                                       \
			}                                                                                         \
			if (__stse_da_tmp_reserve_sliced) {                                                       \
				void *__stse_da_tmp_reserve_items = stse_tmp_alloc((da)->cap * sizeof(*(da)->items)); \
				STSE_ALLOC_ASSERT((da)->items && "could not allocate memory");                        \
				memcpy(__stse_da_tmp_reserve_items, (da)->items, (da)->len * sizeof(*(da)->items));   \
				(da)->items = __stse_da_tmp_reserve_items;                                            \
			} else {                                                                                  \
				(da)->items = stse_tmp_realloc(                                                       \
                    (da)->items,                                                                      \
                    (da)->len * sizeof(*(da)->items),                                                 \
                    (da)->cap * sizeof(*(da)->items));                                                \
				STSE_ALLOC_ASSERT((da)->items && "could not allocate memory");                        \
			}                                                                                         \
		}                                                                                             \
	} while (0)

#define stse_da_tmp_push(da, item)                \
	do {                                          \
		stse_da_tmp_reserve((da), (da)->len + 1); \
		(da)->items[(da)->len++] = (item);        \
	} while (0)

#define stse_da_tmp_pop(da) ((da)->items[(STSE_ASSERT((da)->len > 0), --(da)->len)])

#define stse_da_tmp_insert(da, index, item)                                      \
	do {                                                                         \
		size_t __stse_da_tmp_insert_i = (index);                                 \
		STSE_ASSERT(__stse_da_tmp_insert_i >= 0);                                \
		if (__stse_da_tmp_insert_i >= (da)->len) stse_da_tmp_push((da), (item)); \
		else {                                                                   \
			stse_da_tmp_reserve((da), (da)->len + 1);                            \
			memmove(&(da)->items[__stse_da_tmp_insert_i+1],                      \
					&(da)->items[__stse_da_tmp_insert_i],                        \
					((da)->len - __stse_da_tmp_insert_i)*sizeof(*(da)->items));  \
			(da)->items[__stse_da_tmp_insert_i] = (item);                        \
			(da)->len++;                                                         \
		}                                                                        \
	} while (0)

#define stse_da_tmp_remove(da, index) stse_da_remove(da, index)

#define stse_da_tmp_remove_unordered(da, index) stse_da_remove_unordered(da, index)

#define stse_da_tmp_slice(T, da, start_incl, slice_length) \
    stse_da_slice(T, da, start_incl, slice_length)

#define stse_da_tmp_append_all(da, new_items, count)        \
	do {                                                    \
		if ((count) > 0) {                                  \
			stse_da_tmp_reserve((da), (da)->len + (count)); \
			memcpy((da)->items + (da)->len,                 \
					(new_items),                            \
					(count) * sizeof(*(da)->items));        \
			(da)->len += (count);                           \
		}                                                   \
	} while (0)

#define stse_da_pool_reserve(da, new_cap, pool)                                                      \
	do {                                                                                             \
		if ((da)->cap < (new_cap)) {                                                                 \
			bool __stse_da_pool_reserve_sliced = false;                                              \
			if ((da)->cap == 0) {                                                                    \
				if ((da)->len != 0) __stse_da_pool_reserve_sliced = true;                            \
				(da)->cap = STSE_DA_INITIAL_CAPACITY;                                                \
			}                                                                                        \
			while ((da)->cap < (new_cap)) {                                                          \
				(da)->cap *= 2;                                                                      \
			}                                                                                        \
			if (__stse_da_pool_reserve_sliced) {                                                     \
				void *__stse_da_pool_reserve_items =                                                 \
                    stse_pool_alloc((pool), (da)->cap * sizeof(*(da)->items));                       \
				STSE_ALLOC_ASSERT((da)->items && "could not allocate memory");                       \
				memcpy(__stse_da_pool_reserve_items, (da)->items, (da)->len * sizeof(*(da)->items)); \
				(da)->items = __stse_da_pool_reserve_items;                                          \
			} else {                                                                                 \
				(da)->items = stse_pool_realloc(                                                     \
                    (pool),                                                                          \
                    (da)->items,                                                                     \
                    (da)->len * sizeof(*(da)->items),                                                \
                    (da)->cap * sizeof(*(da)->items));                                               \
				STSE_ALLOC_ASSERT((da)->items && "could not allocate memory");                       \
			}                                                                                        \
		}                                                                                            \
	} while (0)

#define stse_da_pool_push(da, item, pool)                  \
	do {                                                   \
		stse_da_pool_reserve((pool), (da), (da)->len + 1); \
		(da)->items[(da)->len++] = (item);                 \
	} while (0)

#define stse_da_pool_pop(da) stse_da_pop(da)

#define stse_da_pool_insert(da, index, item, pool)                                 \
	do {                                                                           \
		size_t __stse_da_pool_insert_i = (index);                                  \
		STSE_ASSERT(__stse_da_pool_insert_i >= 0);                                 \
		if (__stse_da_pool_insert_i >= (da)->len) stse_da_pool_push((da), (item)); \
		else {                                                                     \
			stse_da_pool_reserve((da), (da)->len + 1);                             \
			memmove(&(da)->items[__stse_da_pool_insert_i+1],                       \
					&(da)->items[__stse_da_pool_insert_i],                         \
					((da)->len - __stse_da_pool_insert_i)*sizeof(*(da)->items));   \
			(da)->items[__stse_da_pool_insert_i] = (item);                         \
			(da)->len++;                                                           \
		}                                                                          \
	} while (0)

#define stse_da_pool_remove(da, index) stse_da_remove(da, index)

#define stse_da_pool_remove_unordered(da, index) stse_da_remove_unordered(da, index)

#define stse_da_pool_slice(T, da, start_incl, slice_length) \
    stse_da_slice(T, da, start_incl, slice_length)

#define stse_da_pool_append_all(da, new_items, count, pool)          \
	do {                                                             \
		if ((count) > 0) {                                           \
			stse_da_pool_reserve((da), (da)->len + (count), (pool)); \
			memcpy((da)->items + (da)->len,                          \
					(new_items),                                     \
					(count) * sizeof(*(da)->items));                 \
			(da)->len += (count);                                    \
		}                                                            \
	} while (0)

/////////////////////
// string builders //
/////////////////////

typedef struct {
	char *items;
	size_t len;
	size_t cap;
} Stse_String_Builder;

STSEDEF int stse_sb_appendf(Stse_String_Builder *sb,
		STSE_FORMAT_STRING const char *fmt, ...) STSE_PRINTF_FORMAT(2, 3);

#define stse_sb_append(sb, c) stse_da_push((sb), (c))
#define stse_sb_insert(sb, i, c) stse_da_insert((sb), (i), (c))
#define stse_sb_append_buf(sb, buf, n) stse_da_append_all((sb), (buf), (n))
#define stse_sb_append_sv(sb, sv) stse_da_append_buf((sb), (sv).data, (sv).len)
#define stse_sb_append_cstr(sb, cstr)                                                       \
	do {                                                                                    \
		const char *__stse_sb_append_cstr_s = (cstr);                                       \
		stse_sb_append_buf((sb), __stse_sb_append_cstr_s, strlen(__stse_sb_append_cstr_s)); \
	} while (0)

#define stse_sb_free(sb) stse_da_free((sb))

//////////////////
// memory pools //
//////////////////

#ifndef STSE_BLOCK_SIZE
#define STSE_BLOCK_SIZE (256*1024)
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

STSEDEF uintptr_t stse_pool_save(Stse_Pool pool);
STSEDEF void stse_pool_rewind(Stse_Pool *pool, uintptr_t save_state);
STSEDEF void *stse_pool_alloc(Stse_Pool *pool, size_t bytes);
STSEDEF void *stse_pool_calloc(Stse_Pool *pool, size_t bytes);
STSEDEF void *stse_pool_realloc(Stse_Pool *pool, void *data, size_t old_size, size_t new_size);
STSEDEF void stse_pool_free(Stse_Pool *pool);

/////////////////
// string view //
/////////////////

#define STSE_SV_FMT "%.*s"
#define STSE_SV_ARG(sv) (int) (sv).len, (sv).data

typedef struct {
	const char *data;
	size_t len;
} Stse_String_View;

STSEDEF Stse_String_View stse_sv_from_cstr(const char *cstr);
STSEDEF const char *stse_sv_to_tmp_cstr(Stse_String_View sv);

STSEDEF Stse_String_View stse_sv_trim_left(Stse_String_View sv);
STSEDEF Stse_String_View stse_sv_trim_left_delim(Stse_String_View sv, char delimiter);
STSEDEF Stse_String_View stse_sv_trim_left_delim_cstr(Stse_String_View sv, const char *delimiters);
STSEDEF Stse_String_View stse_sv_trim_left_delim_sv(Stse_String_View sv, Stse_String_View delimiters);
STSEDEF Stse_String_View stse_sv_trim_left_pred(Stse_String_View sv, int (*predicate)(int));

STSEDEF Stse_String_View stse_sv_trim_right(Stse_String_View sv);
STSEDEF Stse_String_View stse_sv_trim_right_delim(Stse_String_View sv, char delimiter);
STSEDEF Stse_String_View stse_sv_trim_right_delim_cstr(Stse_String_View sv, const char *delimiters);
STSEDEF Stse_String_View stse_sv_trim_right_delim_sv(Stse_String_View sv, Stse_String_View delimiters);
STSEDEF Stse_String_View stse_sv_trim_right_pred(Stse_String_View sv, int (*predicate)(int));

STSEDEF Stse_String_View stse_sv_trim(Stse_String_View sv);
STSEDEF Stse_String_View stse_sv_trim_delim(Stse_String_View sv, char delimiter);
STSEDEF Stse_String_View stse_sv_trim_delim_cstr(Stse_String_View sv, const char *delimiters);
STSEDEF Stse_String_View stse_sv_trim_delim_sv(Stse_String_View sv, Stse_String_View delimiters);
STSEDEF Stse_String_View stse_sv_trim_pred(Stse_String_View sv, int (*predicate)(int));

STSEDEF Stse_String_View stse_sv_tok(Stse_String_View *sv);
STSEDEF Stse_String_View stse_sv_tok_delim(Stse_String_View *sv, char delimiter);
STSEDEF Stse_String_View stse_sv_tok_delim_cstr(Stse_String_View *sv, const char *delimiters);
STSEDEF Stse_String_View stse_sv_tok_delim_sv(Stse_String_View *sv, Stse_String_View delimiters);
STSEDEF Stse_String_View stse_sv_tok_delim_pred(Stse_String_View *sv, int (*is_delim)(int));
STSEDEF Stse_String_View stse_sv_tok_chars_cstr(Stse_String_View *sv, const char *allowed_chars);
STSEDEF Stse_String_View stse_sv_tok_chars_sv(Stse_String_View *sv,Stse_String_View allowed_chars);
STSEDEF bool stse_sv_tok_start(Stse_String_View *sv, Stse_String_View prefix);
STSEDEF bool stse_sv_tok_end(Stse_String_View *sv, Stse_String_View suffix);

STSEDEF bool stse_sv_eq(Stse_String_View sv1, Stse_String_View sv2);
STSEDEF bool stse_sv_starts_with(Stse_String_View sv, Stse_String_View prefix);
STSEDEF bool stse_sv_ends_with(Stse_String_View sv, Stse_String_View suffix);

STSEDEF float stse_fmodf_cycling(float x, float y);
STSEDEF double stse_fmod_cycling(double x, double y);
STSEDEF long double stse_fmodl_cycling(long double x, long double y);

#endif /* STSE_H_ */

#ifdef STSE_IMPLEMENTATION

#ifdef STSE_TMP_POOL
static Stse_Pool stse_tmp_pool = {0};

STSEDEF void stse_tmp_reset(void) {
	stse_pool_free(&stse_tmp_pool);
}

STSEDEF uintptr_t stse_tmp_save(void) {
	return stse_pool_save(stse_tmp_pool);
}

STSEDEF void stse_tmp_rewind(uintptr_t save_state) {
	stse_pool_rewind(&stse_tmp_pool, save_state);
}

STSEDEF void *stse_tmp_alloc(size_t bytes) {
	return stse_pool_alloc(&stse_tmp_pool, bytes);
}

STSEDEF void *stse_tmp_calloc(size_t bytes) {
	return stse_pool_calloc(&stse_tmp_pool, bytes);
}

STSEDEF void *stse_tmp_realloc(void *data, size_t old_size, size_t new_size) {
	return stse_pool_realloc(&stse_tmp_pool, data, old_size, new_size);
}
#else
static char stse_tmp_buffer[STSE_TMP_BUF_CAP] = {0};
static size_t stse_tmp_buffer_size = 0;

STSEDEF void stse_tmp_reset(void) {
	stse_tmp_buffer_size = 0;
}

STSEDEF uintptr_t stse_tmp_save(void) {
	return (uintptr_t) stse_tmp_buffer_size;
}

STSEDEF void stse_tmp_rewind(uintptr_t save_state) {
	stse_tmp_buffer_size = (size_t) save_state;
}

STSEDEF void *stse_tmp_alloc(size_t bytes) {
	size_t word_size = sizeof(uintptr_t);
	bytes = (bytes+word_size-1)/word_size*word_size;
	if (stse_tmp_buffer_size + bytes > STSE_TMP_BUF_CAP)
#ifdef STSE_NO_ASSERT_ALLOCATION
		return NULL;
#else
		STSE_ASSERT(false && "tmp buffer out of memory");
#endif /* STSE_ASSERT_ALLOCATION */
	void *result = &stse_tmp_buffer[stse_tmp_buffer_size];
	stse_tmp_buffer_size += bytes;
	return result;
}

STSEDEF void *stse_tmp_calloc(size_t bytes) {
	size_t word_size = sizeof(uintptr_t);
	bytes = (bytes+word_size-1)/word_size*word_size;
	void *result = stse_tmp_alloc(bytes);

#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!result) return NULL;
#endif /* STSE_ASSERT_ALLOCATION */

	memset(result, 0, bytes);
	return result;
}

STSEDEF void *stse_tmp_realloc(void *data, size_t old_size, size_t new_size) {
	size_t word_size = sizeof(uintptr_t);
	new_size = (new_size + word_size - 1)/word_size*word_size;
	if (new_size <= old_size) return data;

	void *result = stse_tmp_alloc(new_size);
#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!result) return NULL;
#endif /* STSE_ASSERT_ALLOCATION */
	memcpy(result, data, old_size);
	return result;
}
#endif /* STSE_TMP_POOL */

STSEDEF char *stse_tmp_strdup(const char *src) {
	size_t n = strlen(src);
	char *copy = stse_tmp_alloc(n + 1);

#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!copy) return NULL;
#endif /* STSE_ASSERT_ALLOCATION */

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
#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!result) return NULL;
#endif /* STSE_ASSERT_ALLOCATION */
	va_copy(args, ap);
	vsnprintf(result, n+1, format, args);
	va_end(args);

	return result;
}

STSEDEF int stse_sb_appendf(Stse_String_Builder *sb, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	size_t new_cap = sb->len + n + 1;
	stse_da_reserve(sb, new_cap);

	va_start(ap, fmt);
	vsnprintf(sb->items + sb->len, n + 1, fmt, ap);
	va_end(ap);

	sb->len += n;

	return n;
}

STSEDEF uintptr_t stse_pool_save(Stse_Pool pool) {
	if (!pool.blocks) return 0;

	return (uintptr_t) (pool.blocks->data + pool.blocks->empty_offset);
}

STSEDEF void stse_pool_rewind(Stse_Pool *pool, uintptr_t save_state) {
	if (save_state == 0) {
		stse_pool_free(pool);
		return;
	}

	STSE_ASSERT(pool->blocks);
	_Stse_Pool_Block *block = pool->blocks;
	while (block) {
		uintptr_t min = (uintptr_t) block->data;
		uintptr_t max = (uintptr_t) (block->data + block->empty_offset + block->empty_space);

		// not sure if < max would also be okay but dont worry
		if (min <= save_state && save_state <= max) {
			block->empty_offset = save_state - min;
			block->empty_space = max - save_state;
			break;
		}

		_Stse_Pool_Block *next = block->next;
		STSE_FREE(block);
		block = next;
	}

	pool->blocks = block;
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
#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!block) return NULL;
#endif /* STSE_NO_ASSERT_ALLOCATION */

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
	if (new_size <= old_size) return data;

	void *result = stse_pool_alloc(pool, new_size);
#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!result) return NULL;
#endif /* STSE_ASSERT_ALLOCATION */
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

STSEDEF Stse_String_View stse_sv_from_cstr(const char *cstr) {
	return (Stse_String_View) { .data = cstr, .len = strlen(cstr) };
}

STSEDEF const char *stse_sv_to_tmp_cstr(Stse_String_View sv) {
	char *cstr = stse_tmp_alloc(sv.len + 1);

#ifdef STSE_NO_ASSERT_ALLOCATION
	if (!cstr) return NULL;
#endif /* STSE_ASSERT_ALLOCATION */

	memcpy(cstr, sv.data, sv.len);
	cstr[sv.len] = '\0';
	return cstr;
}

STSEDEF Stse_String_View stse_sv_trim_left(Stse_String_View sv) {
	return stse_sv_trim_left_pred(sv, &isspace);
}

STSEDEF Stse_String_View stse_sv_trim_left_delim(Stse_String_View sv, char delimiter) {
	while (sv.len > 0 && sv.data[0] == delimiter) {
		sv.data++;
		sv.len--;
	}

	return sv;
}

STSEDEF Stse_String_View stse_sv_trim_left_delim_cstr(Stse_String_View sv, const char *delimiters) {
	return stse_sv_trim_left_delim_sv(sv, stse_sv_from_cstr(delimiters));
}

STSEDEF Stse_String_View stse_sv_trim_left_delim_sv(Stse_String_View sv, Stse_String_View delimiters) {
	bool done = false;
	while (sv.len > 0 && !done) {
		char c = sv.data[0];
		done = true;
		for (size_t i = 0; i < delimiters.len; i++) {
			if (c == delimiters.data[i]) {
				done = false;
				sv.data++;
				sv.len--;
				break;
			}
		}
	} 

	return sv;
}

STSEDEF Stse_String_View stse_sv_trim_left_pred(Stse_String_View sv, int (*predicate)(int)) {
	while (sv.len > 0 && predicate(sv.data[0])) {
		sv.data++;
		sv.len--;
	}

	return sv;
}

STSEDEF Stse_String_View stse_sv_trim_right(Stse_String_View sv) {
	return stse_sv_trim_right_pred(sv, &isspace);
}

STSEDEF Stse_String_View stse_sv_trim_right_delim(Stse_String_View sv, char delimiter) {
	while (sv.len > 0 && sv.data[sv.len-1] == delimiter) {
		sv.len--;
	}

	return sv;
}

STSEDEF Stse_String_View stse_sv_trim_right_delim_cstr(Stse_String_View sv, const char *delimiters) {
	return stse_sv_trim_right_delim_sv(sv, stse_sv_from_cstr(delimiters));
}

STSEDEF Stse_String_View stse_sv_trim_right_delim_sv(Stse_String_View sv, Stse_String_View delimiters) {
	bool done = false;
	while (sv.len > 0 && !done) {
		char c = sv.data[sv.len - 1];
		done = true;
		for (size_t i = 0; i < delimiters.len; i++) {
			if (c == delimiters.data[i]) {
				done = false;
				sv.len--;
				break;
			}
		}
	} 

	return sv;
}

STSEDEF Stse_String_View stse_sv_trim_right_pred(Stse_String_View sv, int (*predicate)(int)) {
	while (sv.len > 0 && predicate(sv.data[sv.len-1])) {
		sv.len--;
	}

	return sv;
}

STSEDEF Stse_String_View stse_sv_trim(Stse_String_View sv) {
	return stse_sv_trim_left(stse_sv_trim_right(sv));
}

STSEDEF Stse_String_View stse_sv_trim_delim(Stse_String_View sv, char delimiter) {
	return stse_sv_trim_left_delim(stse_sv_trim_right_delim(sv, delimiter), delimiter);
}

STSEDEF Stse_String_View stse_sv_trim_delim_cstr(Stse_String_View sv, const char *delimiters) {
	return stse_sv_trim_left_delim_cstr(stse_sv_trim_right_delim_cstr(sv, delimiters), delimiters);
}

STSEDEF Stse_String_View stse_sv_trim_delim_sv(Stse_String_View sv, Stse_String_View delimiters) {
	return stse_sv_trim_left_delim_sv(stse_sv_trim_right_delim_sv(sv, delimiters), delimiters);
}

STSEDEF Stse_String_View stse_sv_trim_pred(Stse_String_View sv, int (*predicate)(int)) {
	return stse_sv_trim_left_pred(stse_sv_trim_right_pred(sv, predicate), predicate);
}

STSEDEF Stse_String_View stse_sv_tok(Stse_String_View *sv) {
	return stse_sv_tok_delim_pred(sv, &isspace);
}

STSEDEF Stse_String_View stse_sv_tok_delim(Stse_String_View *sv, char delimiter) {
	*sv = stse_sv_trim_left_delim(*sv, delimiter);

	Stse_String_View result = (Stse_String_View) {
		.data = sv->data,
		.len = 0,
	};

	while (sv->len > 0 && sv->data[0] != delimiter) {
		sv->data++;
		sv->len--;
		result.len++;
	}

	return result;
}

STSEDEF Stse_String_View stse_sv_tok_delim_cstr(Stse_String_View *sv, const char *delimiters) {
	return stse_sv_tok_delim_sv(sv, stse_sv_from_cstr(delimiters));
}

STSEDEF Stse_String_View stse_sv_tok_delim_sv(Stse_String_View *sv,  Stse_String_View delimiters) {
	*sv = stse_sv_trim_left_delim_sv(*sv, delimiters);

	Stse_String_View result = (Stse_String_View) {
		.data = sv->data,
		.len = 0,
	};

	while (sv->len > 0) {
		char c = sv->data[0];
		bool done = false;
		for (size_t i = 0; i < delimiters.len; i++) {
			if (c == delimiters.data[i]) {
				done = true;
				break;
			}
		}

		if (done) break;

		sv->data++;
		sv->len--;
		result.len++;
	}  

	return result;
}

STSEDEF Stse_String_View stse_sv_tok_delim_pred(Stse_String_View *sv, int (*is_delim)(int)) {
	*sv = stse_sv_trim_left_pred(*sv, is_delim);

	Stse_String_View result = (Stse_String_View) {
		.data = sv->data,
		.len = 0,
	};

	while (sv->len > 0 && !is_delim(sv->data[0])) {
		sv->data++;
		sv->len--;
		result.len++;
	} 

	return result;
}

STSEDEF Stse_String_View stse_sv_tok_chars_cstr(Stse_String_View *sv, const char *allowed_chars) {
	return stse_sv_tok_chars_sv(sv, stse_sv_from_cstr(allowed_chars));
}

STSEDEF Stse_String_View stse_sv_tok_chars_sv(Stse_String_View *sv, Stse_String_View allowed_chars) {
	while (sv->len > 0) {
		char c = sv->data[0];
		bool done = false;
		for (size_t i = 0; i < allowed_chars.len; i++) {
			if (c == allowed_chars.data[i]) {
				done = true;
				break;
			}
		}

		if (done) break;

		sv->data++;
		sv->len--;
	}  

	Stse_String_View result = (Stse_String_View) {
		.data = sv->data,
		.len = 0,
	};

	bool done = false;
	while (sv->len > 0 && !done) {
		char c = sv->data[0];
		done = true;
		for (size_t i = 0; i < allowed_chars.len; i++) {
			if (c == allowed_chars.data[i]) {
				done = false;
				sv->data++;
				sv->len--;
				result.len++;
				break;
			}
		}
	} 

	return result;
}

STSEDEF bool stse_sv_tok_start(Stse_String_View *sv,  Stse_String_View prefix) {
	if (!stse_sv_starts_with(*sv, prefix)) return false;
	sv->len -= prefix.len;
	sv->data += prefix.len;
	return true;
}

STSEDEF bool stse_sv_tok_end(Stse_String_View *sv,  Stse_String_View suffix) {
	if (!stse_sv_ends_with(*sv, suffix)) return false;
	sv->len -= suffix.len;
	return true;
}

STSEDEF bool stse_sv_eq(Stse_String_View sv1, Stse_String_View sv2) {
	if (sv1.len != sv2.len) return false;
	for (size_t i = 0; i < sv1.len; i++) {
		if (sv1.data[i] != sv2.data[i]) return false;
	}

	return true;
}

STSEDEF bool stse_sv_starts_with(Stse_String_View sv, Stse_String_View prefix) {
	if (sv.len < prefix.len) return false;
	sv.len = prefix.len;
	return stse_sv_eq(sv, prefix);
}

STSEDEF bool stse_sv_ends_with(Stse_String_View sv, Stse_String_View suffix) {
	if (sv.len < suffix.len) return false;
	sv.data += sv.len - suffix.len;
	sv.len = suffix.len;
	return stse_sv_eq(sv, suffix);
}

// All function contain a floor. To avoid either
// an extra macro flag or forcing every user to link with libm,
// the floor implementation from musl is copied:
// MUSL: <musl.libc.org>
// LICENSE:
// ----------------------------------------------------------------------
// Copyright © 2005-2020 Rich Felker, et al.
// 
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------
// 
// comments for clarity are added which are not present in the musl implementation.
// /* */-style comments were alread present
//
// The implementation of the fmod functions boils down to:
// x - y * floor(x / y);

STSEDEF float stse_fmodf_cycling(float x, float y) {
	float frac = x / y;
	// start floor (musl, see license above)
	// 32-bit float = 1 sign bit, 8 exponent bits, 23 mantissa bits
	union { float f; uint32_t i; } u = {frac};
	int e = (int)(u.i >> 23 & 0xff) - 0x7f;
	uint32_t m;

	// leave value untouched when exponent is equal to or greater than mantissa,
	// value is already an integer
	if (e < 23) {
		if (e >= 0) {
			m = 0x007fffff >> e; // mantissa fractional part mask

			if ((u.i & m) != 0) {
				volatile float f = x + 0x1p120f; // i have no idea 
				(void)f;
				// when negative, adding m increases absolute value of integer part by one
				// this makes sure truncation gives correct floor result
				if (u.i >> 31) u.i += m;
				u.i &= ~m; // truncate
			}
		} else {
			// we are in the range (-1, 1)
			// [0, 1) -> 0
			// (-1, 0) -> -1
			// -0 stays -0 i think
			volatile float f = x + 0x1p120f; // i have no idea 
			(void)f;

			if (u.i >> 31 == 0) u.i = 0;
			else if (u.i << 1) u.f = -1.0f; // negative but not zero
		}

		frac = u.f;
	}
	// end floor
	
	return x - y * frac;
}

STSEDEF double stse_fmod_cycling(double x, double y) {
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
	static const double toint = 1/DBL_EPSILON;
	double z;
#elif FLT_EVAL_METHOD==2
	static const long double toint = 1/LDBL_EPSILON;
	long double z;
#endif

	double frac = x / y;

	// start floor
	// 64-bit float = 1 sign bit, 11 exponent bits, 52 mantissa bits
	union { double f; uint64_t i; } u = { frac };
	int e = u.i >> 52 & 0x7ff;
	// do nothing if value is an integer or zero
	if (e < 0x3ff + 52 && frac != 0) {
		/* z = int(frac) - frac, where int(frac) is an integer neighbor of frac */
		if (u.i >> 63) z = frac - toint + toint - frac;
		else z = frac + toint - toint - frac;

		/* special case because of non-nearest rounding modes */
		if (e <= 0x3ff-1) {
#if FLT_EVAL_METHOD==0 || FLT_EVAL_METHOD==1
			volatile double zz = z;
#elif FLT_EVAL_METHOD==2
			volatile long double zz = z;
#endif
			(void)zz;
			if (u.i >> 63) {
				frac = -1.0;
			} else {
				frac = 0.0;
			}
		} else {
			if (z > 0) {
				frac = frac + z - 1;
			} else {
				frac = frac + z;
			}
		}
	}
	// end floor

	return x - y * frac;
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024

STSEDEF long double stse_fmodl_cycling(long double x, long double y) {
	return stse_fmod_cycling(x, y);
}

#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384

STSEDEF long double stse_fmodl_cycling(long double x, long double y) {

#if LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __LITTLE_ENDIAN
union ldshape {
	long double f;
	struct {
		uint64_t m;
		uint16_t se;
	} i;
};
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __BIG_ENDIAN
/* This is the m68k variant of 80-bit long double, and this definition only works
 * on archs where the alignment requirement of uint64_t is <= 4. */
union ldshape {
	long double f;
	struct {
		uint16_t se;
		uint16_t pad;
		uint64_t m;
	} i;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __LITTLE_ENDIAN
union ldshape {
	long double f;
	struct {
		uint64_t lo;
		uint32_t mid;
		uint16_t top;
		uint16_t se;
	} i;
	struct {
		uint64_t lo;
		uint64_t hi;
	} i2;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && __BYTE_ORDER == __BIG_ENDIAN
union ldshape {
	long double f;
	struct {
		uint16_t se;
		uint16_t top;
		uint32_t mid;
		uint64_t lo;
	} i;
	struct {
		uint64_t hi;
		uint64_t lo;
	} i2;
};
#else
#error Unsupported long double representation
#endif
	long double frac = x / y;

	// start floor
	static const long double toint = 1/LDBL_EPSILON;

	union ldshape u = {frac};
	int e = u.i.se & 0x7fff;
	long double z;

	if (e < 0x3fff+LDBL_MANT_DIG-1 && frac != 0) {
		/* z = int(frac) - frac, where int(frac) is an integer neighbor of frac */
		if (u.i.se >> 15) z = frac - toint + toint - frac;
		else z = frac + toint - toint - frac;
		/* special case because of non-nearest rounding modes */
		if (e <= 0x3fff-1) {
			volatile long double zz = z;
			(void)zz;
			if (u.i.se >> 15) frac = -1;
			else frac = 0;
		} else {
			if (z > 0) frac = frac + z - 1;
			else frac = frac + z;
		}
	}

	// end floor

	return x - y * frac;
}

#endif


#endif /* STSE_IMPLEMENTATION */

#ifdef STSE_OMIT_NS
#define ARRAY_LEN STSE_ARRAY_LEN
#define SWAP STSE_SWAP
#define SHIFT STSE_SHIFT
#define da_reserve stse_da_reserve
#define da_push stse_da_push
#define da_pop stse_da_pop
#define da_insert stse_da_insert
#define da_remove stse_da_remove
#define da_remove_unordered stse_da_remove_unordered
#define da_slice stse_da_slice
#define da_append_all stse_da_append_all
#define da_free stse_da_free
#define da_tmp_reserve stse_da_tmp_reserve
#define da_tmp_push stse_da_tmp_push
#define da_tmp_pop stse_da_tmp_pop
#define da_tmp_insert stse_da_tmp_insert
#define da_tmp_remove stse_da_tmp_remove
#define da_tmp_remove_unordered stse_da_tmp_remove_unordered
#define da_tmp_slice stse_da_tmp_slice
#define da_tmp_append_all stse_da_tmp_append_all
#define da_pool_reserve stse_da_pool_reserve
#define da_pool_push stse_da_pool_push
#define da_pool_pop stse_da_pool_pop
#define da_pool_insert stse_da_pool_insert
#define da_pool_remove stse_da_pool_remove
#define da_pool_remove_unordered stse_da_pool_remove_unordered
#define da_pool_slice stse_da_pool_slice
#define da_pool_append_all stse_da_pool_append_all
#define String_Builder Stse_String_Builder
#define sb_appendf stse_sb_appendf
#define sb_append stse_sb_append
#define sb_insert stse_sb_insert
#define sb_append_buf stse_sb_append_buf
#define sb_append_sv stse_sb_append_sv
#define sb_append_cstr stse_sb_append_cstr
#define sb_free stse_sb_free
#define tmp_reset stse_tmp_reset
#define tmp_save stse_tmp_save
#define tmp_rewind stse_tmp_rewind
#define tmp_alloc stse_tmp_alloc
#define tmp_calloc stse_tmp_calloc
#define tmp_realloc stse_tmp_realloc
#define tmp_strdup stse_tmp_strdup
#define tmp_strndup stse_tmp_strndup
#define tmp_sprintf stse_tmp_sprintf
#define tmp_vsprintf stse_tmp_vsprintf
#define Pool Stse_Pool
#define pool_save stse_pool_save
#define pool_rewind stse_pool_rewind
#define pool_alloc stse_pool_alloc
#define pool_calloc stse_pool_calloc
#define pool_realloc stse_pool_realloc
#define pool_free stse_pool_free
#define SV_FMT STSE_SV_FMT
#define SV_ARG STSE_SV_ARG
#define String_View Stse_String_View
#define sv_from_cstr stse_sv_from_cstr
#define sv_to_tmp_cstr stse_sv_to_tmp_cstr
#define sv_trim_left stse_sv_trim_left
#define sv_trim_left_delim stse_sv_trim_left_delim
#define sv_trim_left_delim_cstr stse_sv_trim_left_delim_cstr
#define sv_trim_left_delim_sv stse_sv_trim_left_delim_sv
#define sv_trim_left_pred stse_sv_trim_left_pred
#define sv_trim_right stse_sv_trim_right
#define sv_trim_right_delim stse_sv_trim_right_delim
#define sv_trim_right_delim_cstr stse_sv_trim_right_delim_cstr
#define sv_trim_right_delim_sv stse_sv_trim_right_delim_sv
#define sv_trim_right_pred stse_sv_trim_right_pred
#define sv_trim stse_sv_trim
#define sv_trim_delim stse_sv_trim_delim
#define sv_trim_delim_cstr stse_sv_trim_delim_cstr
#define sv_trim_delim_sv stse_sv_trim_delim_sv
#define sv_trim_pred stse_sv_trim_pred
#define sv_tok stse_sv_tok
#define sv_tok_delim stse_sv_tok_delim
#define sv_tok_delim_cstr stse_sv_tok_delim_cstr
#define sv_tok_delim_sv stse_sv_tok_delim_sv
#define sv_tok_delim_pred stse_sv_tok_delim_pred
#define sv_tok_chars_cstr stse_sv_tok_chars_cstr
#define sv_tok_chars_sv stse_sv_tok_chars_sv
#define sv_tok_start stse_sv_tok_start
#define sv_tok_end stse_sv_tok_end
#define sv_eq stse_sv_eq
#define sv_starts_with stse_sv_starts_with
#define sv_ends_with stse_sv_ends_with
#define fmodf_cycling stse_fmodf_cycling
#define fmod_cycling  stse_fmod_cycling
#define fmodl_cycling stse_fmodl_cycling
#endif /* STSE_OMIT_NS */
