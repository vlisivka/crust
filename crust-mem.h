// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_MEM_H_
#define CRUST_MEM_H_

#include <stdlib.h>
#include <sys/types.h>

#ifdef _CRUST_TESTS
#include <stdint.h>
/* Includes for built-in tests. */
#include "crust-unittest.h"
#endif

//
// Shortcuts for attributes
//

/** Warn user about unused result of function. */
#define WUR __attribute__((warn_unused_result))

/** Function may be unused, so compiler should not show warning about unused static inline function. */
#define MU __attribute__((unused))

/** All function arguments cannot be NULL. */
#define NN __attribute__((nonnull))

/** Shortcut for static inline. */
#define SI static inline


/**
 * GCC and Clang extension to call cleanup function at exit
 * from the function with pointer to given function as argument.
 */
#define defer(func) __attribute__((cleanup(func)))
#ifdef _CRUST_TESTS
NN MU SI void mem_charp_destroy(char * * value) {
  if(*value) {
    free(*value);
    *value = NULL;
  }
}

it(defer, "must deallocate object at the end of the function") {
  defer(mem_charp_destroy) char * s = calloc(1, sizeof(char));
  (void)s;
}
#endif

/**
 * Error codes for mem_panic().
 */
enum Mem_error_codes {
  MEM_ERROR_INTEGER_OVERFLOW = 1,//!< MEM_ERROR_INTEGER_OVERFLOW
  MEM_ERROR_OUT_OF_MEM = 2,      //!< MEM_ERROR_OUT_OF_MEM
};

/**
 * Print error message and abort program.
 */
void mem_panic(int error_code, size_t value);
#ifdef _CRUST_TESTS
  it(mem_panic, "must abort program") {
    assert_abort(mem_panic(MEM_ERROR_INTEGER_OVERFLOW, 1), "must abort");
    assert_abort(mem_panic(MEM_ERROR_INTEGER_OVERFLOW, 2), "must abort");
    assert_abort(mem_panic(MEM_ERROR_INTEGER_OVERFLOW, 123123), "must abort");
  }
#endif

/**
 * Checks for integer overflow and out of memory.
 */
WUR void * mem_realloc(void *ptr, size_t length, size_t element_size);
#ifdef _CRUST_TESTS
  it(mem_realloc__1, "must allocate new memory") {
    defer(mem_charp_destroy) char * s = mem_realloc(NULL, 100, 3);
    assert_true(s!=NULL, "mem_realloc() must allocate memory");
  }

  it(mem_realloc__2, "must reallocate old memory") {
    defer(mem_charp_destroy) char * s = mem_realloc(NULL, 2, 2);
    s = mem_realloc(s, 100, 100);
    assert_true(s!=NULL, "mem_realloc() must extended memory block as necessary");
  }

  it(mem_realloc__3, "must panic at integer overflow") {
    char * s = NULL;
    assert_abort(s = mem_realloc(NULL, SIZE_MAX/2, 5), "must abort at integer overflow");
    (void)s;
  }

  /* Triggers valgrind warning about "fishy value", so muted out.*/
//  it(mem_realloc__4, "must panic at out of memory") {
//    char * s = NULL;
//    assert_abort(s = mem_realloc(NULL, SIZE_MAX, 1), "must abort at out of memory");
//    (void)s;
//  }
#endif

/**
 * Checks for integer overflow and out of memory.
 */
WUR MU SI void * mem_malloc(size_t length, size_t element_size) {
  return mem_realloc(NULL, length, element_size);
}
#ifdef _CRUST_TESTS
  it(mem_malloc__1, "must allocate new memory") {
    defer(mem_charp_destroy) char * s = mem_malloc(100, 3);
    assert_true(s!=NULL, "mem_malloc() must allocate memory");
  }
#endif

/**
 * Checks for integer overflow and out of memory.
 */
WUR void * mem_calloc(size_t length, size_t element_size);
#ifdef _CRUST_TESTS
  it(mem_calloc__1, "must allocate new memory") {
    defer(mem_charp_destroy) char * s = mem_calloc(100, 3);
    assert_true(s!=NULL, "mem_calloc() must allocate memory");
  }

  it(mem_calloc__2, "must panic at integer overflow") {
    char * s = NULL;
    assert_abort(s = mem_calloc(SIZE_MAX/2, 5), "must abort at integer overflow");
    (void)s;
  }

  /* Triggers valgrind warning about "fishy value", so muted out.*/
//  it(mem_calloc__3, "must panic at out of memory") {
//    char * s = NULL;
//    assert_abort(s = mem_calloc(SIZE_MAX, 1), "must abort at out of memory");
//    (void)s;
//  }
#endif

#endif
