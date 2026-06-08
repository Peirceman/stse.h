# STSE.h
Stuff that should exist in c, basically an expansion for the standard library.
**HEAVILY** inspired by [nob.h](https://github.com/tsoding/nob.h).

## features
more might come, but it will depend on what i need in the future.
 - some basic macros
 - a temporary buffer
 - dynamic arrays
 - string builders
 - memory pool/memory arena
 - string views

## config macros
macros that users can modify in their own code, they must be defined before including the library.
 - `STSEDEF`: gets put in front of every function definition eg. static inline, default: ``
 - `STSE_REALLOC`: which function to use for realloc calls, default: `realloc`
 - `STSE_FREE`: which function to use for free calls, default: `free`
 - `STSE_ASSERT`: which function to use for assert calls, default: `assert`
 - `STSE_TMP_BUF_CAP`: capacity (in bytes) for the temporary buffer, default: 4mb
 - `STSE_BLOCK_SIZE`: default size (in bytes) for memory blocks used in memory pools, default: 256kb
 - `STSE_DA_INITIAL_CAPACITY`: initial capacity (in elements) for dynamic arrays, default: 256
 - `STSE_OMIT_NS`: omits `stse` in front of functions, default: not set
 - `STSE_IMPLEMENTATION`: this library in in the stile of stb header-only libraries. The header file
       both defines and implements all functions. If you define this macro, the implementations
       are included in the current source file. Usually this should only be done in one file, unless
       you know what you're doing (eg. when `STSEDEF` is set to `static`), default: not set
 - `STSE_NO_ASSERT_ALLOCATION`: when unset, assert all memory allocations are valid (not NULL).
       If set, return NULL in functions that deal with allocations (other functions in stse may
       still assert allocations are succesfull, even when this is set), default: unset
 - `STSE_TMP_POOL`: when set, use the defined `Stse_Pool` for the tmp buffer instead of a static
       array. If this value is set, `STSE_TMP_BUF_CAP` has no effect. default: unset
