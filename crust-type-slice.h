// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_TYPE_SLICE_H_
#define CRUST_TYPE_SLICE_H_

#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>

#include "crust-mem.h"

#ifdef _CRUST_TESTS
/* Includes for built-in test cases */
#include "crust-unittest.h"

#include "crust-type-int.h"
#include "crust-type-array.h"
#include "crust-type-option.h"
#endif

enum _Slice_error_codes {
  _SLICE_ERROR_NO_DATA = 1,
  _SLICE_ERROR_INDEX_OUT_OF_BOUNDS = 2,
};

void _slice_panic(int error_code, size_t index);
#ifdef _CRUST_TESTS
it(_slice_panic, "must abort program using abort()") {
  assert_abort(_slice_panic(_SLICE_ERROR_NO_DATA, 1), "must abort");
  assert_abort(_slice_panic(_SLICE_ERROR_INDEX_OUT_OF_BOUNDS, 1), "must abort");
  assert_abort(_slice_panic(12312, 1), "must abort");
}
#endif

/**
 * Slice is basic wrapper for array.
 *
 * @member data is pointer to array;
 * @member count is length of array in array units (not size of array in bytes).
 *
 * Data is not owned by slice, so destroy() method does nothing and clone()
 * is always shallow. For owned data, vector is better alternative, so, to
 * own data, user must call from_slice() or to_vec(), because just clone()
 * is not enough.
 *
 * Pointer can be increased up (with decreasing of length) until end of slice.
 * Length can be decreased until zero length.
 * Raw data can be accessed and modified, if necessary, but usually slice is
 * readonly.
 *
 * In general, functions should accept slice(s) and return vector.
 */
typedef struct _Slice_s {
  const void * data;
  size_t count;
} _Slice;

/** Construct new slice from raw pointer and length.
 * It's allowed to create new slice of zero length with NULL data pointer.
 * Panics when data==NULL, but length!=0. */
WUR MU SI struct _Slice_s _slice_from_raw_parts(const void * data, size_t length) {
  if(data==NULL && length!=0) {
      _slice_panic(_SLICE_ERROR_NO_DATA, length);
  }
  return (_Slice) { .count = length, .data = data };
}
#ifdef _CRUST_TESTS
it(_slice_from_raw_parts, "must create slice from raw pointer and length") {
  int data[] = { 0, 1, 2, 3, 4 };
  _Slice slice = _slice_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(5, slice.count, "_slice_from_raw_parts must initialize count field to length of array");
  assert_true(data == slice.data, "_slice_from_raw_parts must initialize data pointer by pointer to array");

  slice = _slice_from_raw_parts(NULL, 0);
  assert_equal_int(0, slice.count, "_slice_from_raw_parts must initialize count field to length of array");
  assert_true(NULL == slice.data, "_slice_from_raw_parts must initialize data pointer by pointer to array");

  assert_abort(slice = _slice_from_raw_parts(NULL, 1), "must panic when data==NULL, but length!=0");
}
#endif

/** Create copy of slice. Array is not copied or cloned. */
WUR MU SI _Slice _slice_clone(const _Slice other) { return _slice_from_raw_parts(other.data, other.count); }
#ifdef _CRUST_TESTS
it(_slice_clone, "must create shallow copy of slice") {
  int data[] = { 0, 1, 2, 3, 4 };
  _Slice slice = _slice_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  _Slice slice_copy = _slice_clone(slice);
  assert_equal_int(5, slice_copy.count, "_slice_clone() must create copy");
  assert_true(slice_copy.data == data, "_slice_clone() must create shallow copy");
}
#endif

/** Set length to 0 and pointer to NULL. Array is not freed or destroyed. */
NN MU SI void _slice_destroy(_Slice * self) { self->count = 0; self->data = NULL; }
#ifdef _CRUST_TESTS
it(_slice_destroy, "must erase slice variables") {
  int data[] = { 0, 1, 2, 3, 4 };
  _Slice slice = _slice_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  _slice_destroy(&slice);
  assert_equal_int(0, slice.count, "_slice_destroy() must set length to 0");
  assert_true(NULL == slice.data, "_slice_destroy() must set data pointer to NULL");
  _slice_destroy(&slice); // Safe to call twice
}
#endif

/** Return length of slice in array units, not size in bytes. */
NN WUR MU SI size_t _slice_len(const _Slice * self) { return self->count; }
#ifdef _CRUST_TESTS
it(_slice_len, "must return length of array in array units (not size in bytes)") {
  int data[] = { 0, 1, 2, 3, 4 };
  _Slice slice = _slice_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(5, _slice_len(&slice), "_slice_len() must return length of array");
}
#endif

#define DEFINE_SLICE_STRUCT(SELFNAME) \
/** Wrapper structure around generic slice structure for type safety.
 * Slice of type int is not compatible with slice of type char. */ \
typedef struct { \
  _Slice super; \
} SELFNAME;
#ifdef _CRUST_TESTS
DEFINE_SLICE_STRUCT(Slice_int)

/** Print content of slice for debugging purposes. */
NN MU SI void slice_int_debug(Slice_int * slice) {
  _Slice * super = &slice->super;
  size_t len = super->count;
  int * data = (int *)super->data;
  printf("Slice_int[len=%zu]={", len);
  if(len > 0) {
    for(size_t i=0; i < len-1; i++) {
      printf("%d, ", data[i]);
    }
    printf("%d", data[len-1]);
  }
  printf("}\n");
}
#endif

#define DEFINE_SLICE_FROM_RAW_PARTS(SELFNAME, SELFPREFIX, CTYPE) \
/** Create slice from raw pointer to array and array length in array units (not size in bytes).*/ \
WUR MU SI SELFNAME SELFPREFIX##_from_raw_parts(const CTYPE * data, size_t length) { return (SELFNAME) { .super = _slice_from_raw_parts(data, length) }; }
#ifdef _CRUST_TESTS
DEFINE_SLICE_FROM_RAW_PARTS(Slice_int, slice_int, int)
it(slice_int_from_raw_parts, "must create slice from typed pointer and length") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(5, slice.super.count, "slice_int_from_raw_parts must initialize count field to length of array");
  assert_true(slice.super.data == data, "slice_int_from_raw_parts must initialize data pointer by pointer to array");
}
#endif

#define DEFINE_SLICE_DESTROY(SELFNAME, SELFPREFIX) \
/** Set length of slice to 0 and data pointer to 0. */ \
NN MU SI void SELFPREFIX##_destroy(SELFNAME * self) { _slice_destroy(&self->super); }
#ifdef _CRUST_TESTS
DEFINE_SLICE_DESTROY(Slice_int, slice_int)
it(slice_int_destroy, "must erase slice length and data pointer") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  slice_int_destroy(&slice);
  assert_equal_int(0, slice.super.count, "slice_int_destroy() must set length to 0");
  assert_true(NULL == slice.super.data, "slice_int_destroy() must set data pointer to 0");
  slice_int_destroy(&slice); // Safe to call twice
}
#endif

#define DEFINE_SLICE_DEFAULT(SELFNAME, SELFPREFIX) \
/** Create empty slice with length set to 0 and data pointer set to NULL. */ \
WUR MU SI SELFNAME SELFPREFIX##_default() { return (SELFNAME) { .super = _slice_from_raw_parts(NULL, 0) }; }
#ifdef _CRUST_TESTS
DEFINE_SLICE_DEFAULT(Slice_int, slice_int)
it(slice_int_default, "must return empty slice") {
  Slice_int slice = slice_int_default();
  assert_equal_int(0, slice.super.count, "slice_int_destroy() must set length to 0");
  assert_true(NULL == slice.super.data, "slice_int_destroy() must set data pointer to 0");
  slice_int_destroy(&slice); // Safe to call on empty slice
}
#endif

#define DEFINE_SLICE_LEN(SELFNAME, SELFPREFIX) \
/** Return length of slice (not size in bytes). */ \
NN WUR MU SI size_t SELFPREFIX##_len(const SELFNAME * self) { return _slice_len(&self->super); }
#ifdef _CRUST_TESTS
DEFINE_SLICE_LEN(Slice_int, slice_int)
it(slice_int_len, "must return slice length") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(5, slice_int_len(&slice), "must return length");
}
#endif

#define DEFINE_SLICE_AS_PTR(SELFNAME, SELFPREFIX, CTYPE) \
/** Return typed pointer to data. */ \
NN WUR MU SI CTYPE * SELFPREFIX##_as_ptr(const SELFNAME * self) { \
  const _Slice * super = &self->super; \
  return (CTYPE *) super->data; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_AS_PTR(Slice_int, slice_int, int)
it(slice_int_as_ptr, "must return typed pointer to data") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_true(data == slice_int_as_ptr(&slice), "must return typed pointer to data");
}
#endif

#define DEFINE_SLICE_CLONE(SELFNAME, SELFPREFIX) \
/** Return shallow copy of slice. */ \
WUR MU SI SELFNAME SELFPREFIX##_clone(const SELFNAME other) { return (SELFNAME) { .super = _slice_clone(other.super) }; }
#ifdef _CRUST_TESTS
DEFINE_SLICE_CLONE(Slice_int, slice_int)
it(slice_int_clone, "must return copy of slice") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  Slice_int slice_copy = slice_int_clone(slice);
  assert_equal_int(5, slice_int_len(&slice_copy), "slice_int_clone() must return copy");
  assert_true(data == slice_int_as_ptr(&slice_copy), "slice_int_clone() must return copy");
}
#endif

#define DEFINE_SLICE_GET(SELFNAME, SELFPREFIX, CTYPE) \
/** Return item by index. Panics if index is out of bounds. */ \
NN WUR MU SI CTYPE SELFPREFIX##_get(const SELFNAME * self, size_t index) { \
  const _Slice * super = &self->super; \
 \
  if(index >= super->count) { \
    _slice_panic(_SLICE_ERROR_INDEX_OUT_OF_BOUNDS, index); \
  } \
 \
  return SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_GET(Slice_int, slice_int, int)
it(slice_int_get, "must return item, and must panic if index out of bounds") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(3, slice_int_get(&slice, 3), "must return item");
  assert_abort((void)(0 == slice_int_get(&slice, 6)), "must panic");
}
#endif

#define DEFINE_SLICE_GET_UNCHECKED(SELFNAME, SELFPREFIX, CTYPE) \
/** Return item by index without checks. */ \
NN WUR MU SI CTYPE SELFPREFIX##_get_unchecked(const SELFNAME * self, size_t index) { \
  return SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_GET_UNCHECKED(Slice_int, slice_int, int)
it(slice_int_get_unchecked, "must return item, and must not panic if index out of bounds") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(3, slice_int_get_unchecked(&slice, 3), "must return item");

  int result = slice_int_get_unchecked(&slice, 6);
  (void)result;
}
#endif


#define DEFINE_SLICE_GET_MUT(SELFNAME, SELFPREFIX, CTYPE) \
/** Return pointer to item by index. Panics if index is out of bounds. */ \
NN WUR MU SI CTYPE * SELFPREFIX##_get_mut(const SELFNAME * self, size_t index) { \
  const _Slice * super = &self->super; \
 \
  if(index >= super->count) { \
    _slice_panic(_SLICE_ERROR_INDEX_OUT_OF_BOUNDS, index); \
  } \
 \
  return &SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_GET_MUT(Slice_int, slice_int, int)
it(slice_int_get_mut, "must return pointer to item, and must panic if index out of bounds") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(3, *slice_int_get_mut(&slice, 3), "must return pointer to item");
  assert_abort((void)(0 == slice_int_get_mut(&slice, 6)), "must panic");
}
#endif

#define DEFINE_SLICE_GET_UNCHECKED_MUT(SELFNAME, SELFPREFIX, CTYPE) \
/** Return pointer to item by index without checks. */ \
NN WUR MU SI CTYPE * SELFPREFIX##_get_unchecked_mut(const SELFNAME * self, size_t index) { \
  return &SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_GET_UNCHECKED_MUT(Slice_int, slice_int, int)
it(slice_int_get_unchecked_mut, "must return pointer item, and must not panic if index out of bounds") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(3, *slice_int_get_unchecked_mut(&slice, 3), "must return pointer to item");

  int result = *slice_int_get_unchecked_mut(&slice, 6);
  (void)result;
}
#endif

#define DEFINE_SLICE_GET_OR_DEFAULT(SELFNAME, SELFPREFIX, CTYPE) \
/** Return item by index or default value. */ \
NN WUR MU SI CTYPE SELFPREFIX##_get_or_default(const SELFNAME * self, size_t index, CTYPE default_value) { \
  const _Slice * super = &self->super; \
 \
  if(index >= super->count) { \
    return default_value; \
  } \
 \
  return SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_GET_OR_DEFAULT(Slice_int, slice_int, int)
it(slice_int_get_or_default, "must return item or default value") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_equal_int(3, slice_int_get_or_default(&slice, 3, 42), "must return item");
  assert_equal_int(42, slice_int_get_or_default(&slice, 6, 42), "must return default value");
}
#endif

#define DEFINE_SLICE_IS_EMPTY(SELFNAME, SELFPREFIX) \
/** Return true if slice is empty.*/ \
NN WUR MU SI bool SELFPREFIX##_is_empty(const SELFNAME * self) { \
  return SELFPREFIX##_len(self) == 0; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_IS_EMPTY(Slice_int, slice_int)
it(slice_int_is_empty, "must return true if slice is empty, false if not") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_true(!slice_int_is_empty(&slice), "must return false when slice is not empty");

  slice = slice_int_from_raw_parts(NULL, 0);
  assert_true(slice_int_is_empty(&slice), "must return true when slice is empty");
}
#endif

#define DEFINE_SLICE_SWAP(SELFNAME, SELFPREFIX, CTYPE) \
/** Swaps two elements in place.*/ \
NN MU SI void SELFPREFIX##_swap(SELFNAME * self, size_t a, size_t b) { \
  CTYPE tmp = SELFPREFIX##_get(self, a); \
  * SELFPREFIX##_get_mut(self, a) = SELFPREFIX##_get(self, b); \
  * SELFPREFIX##_get_unchecked_mut(self, b) = tmp; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_SWAP(Slice_int, slice_int, int)
it(slice_int_swap, "must swap two elements in place") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  slice_int_swap(&slice, 2, 4);
  assert_equal_int(4, slice_int_get(&slice, 2), "must swap");
  assert_equal_int(2, slice_int_get(&slice, 4), "must swap");
}
#endif

#define DEFINE_SLICE_REVERSE(SELFNAME, SELFPREFIX) \
/** Reverse order of elements in place.*/ \
NN MU SI void SELFPREFIX##_reverse(SELFNAME * self) { \
  size_t len = SELFPREFIX##_len(self); \
  for(size_t i=0; i < len/2; i++) { \
      SELFPREFIX##_swap(self, i, len-i-1); \
  } \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_REVERSE(Slice_int, slice_int)
it(slice_int_reverse, "must reverse order of elements in place") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  slice_int_reverse(&slice);
  assert_equal_int(4, slice_int_get(&slice, 0), "must swap");
  assert_equal_int(3, slice_int_get(&slice, 1), "must swap");
  assert_equal_int(2, slice_int_get(&slice, 2), "must swap");
  assert_equal_int(1, slice_int_get(&slice, 3), "must swap");
  assert_equal_int(0, slice_int_get(&slice, 4), "must swap");
}
#endif

#define DEFINE_SLICE_SLICE(SELFNAME, SELFPREFIX) \
/** Create subslice from slice. No data is copied.
 * Panics if start index or start+length is out of bounds. */ \
NN WUR MU SI SELFNAME SELFPREFIX##_slice(const SELFNAME * self, size_t start, size_t length) { \
  size_t len = SELFPREFIX##_len(self); \
  if(start >= len || length > len || start+length > len) { \
      _slice_panic(_SLICE_ERROR_INDEX_OUT_OF_BOUNDS, start+length); \
  } \
  return SELFPREFIX##_from_raw_parts(SELFPREFIX##_get_mut(self, start), length); \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_SLICE(Slice_int, slice_int)
it(slice_int_slice, "must return subslice") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  Slice_int subslice = slice_int_slice(&slice, 2, 3);
  assert_equal_int(2, slice_int_get(&subslice, 0), "must create subslice");
  assert_equal_int(3, slice_int_get(&subslice, 1), "must create subslice");
  assert_equal_int(4, slice_int_get(&subslice, 2), "must create subslice");

  assert_abort(subslice = slice_int_slice(&slice, (size_t)-1, 2), "must abort on integer overflow");
  assert_abort(subslice = slice_int_slice(&slice, 3, 3), "must abort on out of bounds");
}
#endif

#define DEFINE_SLICE_EQ(SELFNAME, SELFPREFIX, TYPEPREFIX) \
/** Compare length and elements of both slices using eq(). */ \
NN WUR MU SI bool SELFPREFIX##_eq(const SELFNAME * self, const SELFNAME * other) { \
  size_t len = SELFPREFIX##_len(self); \
  if(len != SELFPREFIX##_len(other)) \
    return false; \
  if(len == 0) \
    return true; \
  for(size_t i=0; i<len; i++) { \
      if( ! TYPEPREFIX##_eq( SELFPREFIX##_get_unchecked_mut(self, i), SELFPREFIX##_get_unchecked_mut(other, i)) ) \
	return false; \
  } \
  return true; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_EQ(Slice_int, slice_int, int)
it(slice_int_eq, "must compare two slices") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice1 = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  Slice_int slice2 = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  assert_true(slice_int_eq(&slice1, &slice2), "two slices are equal");

  slice2 = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data)-1);
  assert_true(!slice_int_eq(&slice1, &slice2), "two slices with different lengths are not equal");

  int data2[] = { 0, 1, 2, 4 };
  slice1 = slice_int_from_raw_parts(data2, LENGTH_OF_ARRAY(data2));
  assert_true(!slice_int_eq(&slice1, &slice2), "two slices with same length but different elements are not equal");

  slice2 = slice_int_from_raw_parts(NULL, 0);
  assert_true(!slice_int_eq(&slice1, &slice2), "empty and non-empty slice are not equal");

  slice1 = slice_int_from_raw_parts(NULL, 0);
  assert_true(slice_int_eq(&slice1, &slice2), "two empty slices are equal");
}
#endif

#define DEFINE_SLICE_CONTAINS(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
/** Return true if element is found in slice. */ \
NN WUR MU SI bool SELFPREFIX##_contains(const SELFNAME * self, const CTYPE element) { \
  size_t len = SELFPREFIX##_len(self); \
  if(len == 0) \
    return false; \
  for(size_t i=0; i<len; i++) { \
      if( TYPEPREFIX##_eq( SELFPREFIX##_get_unchecked_mut(self, i), &element) ) \
	return true; \
  } \
  return false; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_CONTAINS(Slice_int, slice_int, int, int)
it(slice_int_contains, "must return true when element is found in slice") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));

  assert_true(slice_int_contains(&slice, 4), "slice contains it");
  assert_true(!slice_int_contains(&slice, 5), "slice doesn't contain it");

  slice_int_destroy(&slice);
  assert_true(!slice_int_contains(&slice, 4), "slice is empty");
}
#endif

#define DEFINE_SLICE_STARTS_WITH(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
/** Return true when prefix is found at begin of the slice. */ \
NN WUR MU SI bool SELFPREFIX##_starts_with(const SELFNAME * self, const SELFNAME * prefix) { \
  if(SELFPREFIX##_is_empty(prefix)) return true; \
  if(SELFPREFIX##_is_empty(self)) return false; \
  size_t len = SELFPREFIX##_len(prefix); \
  if(len > SELFPREFIX##_len(self)) return false; \
  for(size_t i = 0; i < len; i++) { \
      if(!TYPEPREFIX##_eq(SELFPREFIX##_get_unchecked_mut(self, i), SELFPREFIX##_get_unchecked_mut(prefix, i))) return false; \
  } \
  return true; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_STARTS_WITH(Slice_int, slice_int, int, int)
it(slice_int_starts_with, "must return true when prefix matches beginning of the slice") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));

  {
    int prefix_data[] = { 0, 1, 2 };
    Slice_int prefix = slice_int_from_raw_parts(prefix_data, LENGTH_OF_ARRAY(prefix_data));
    assert_true(slice_int_starts_with(&slice, &prefix), "slice contains it");
  }

  {
    int prefix_data[] = { 0, 1, 2, 3, 4 };
    Slice_int prefix = slice_int_from_raw_parts(prefix_data, LENGTH_OF_ARRAY(prefix_data));
    assert_true(slice_int_starts_with(&slice, &prefix), "slice contains it");
  }

  {
    Slice_int prefix = slice_int_default();
    assert_true(slice_int_starts_with(&slice, &prefix), "slice contains it");
  }

  {
    Slice_int prefix = slice_int_from_raw_parts(NULL, 0);
    assert_true(slice_int_starts_with(&slice, &prefix), "slice always contains empty prefix");
  }

  int prefix_data[] = { 1, 2 };
  Slice_int prefix = slice_int_from_raw_parts(prefix_data, LENGTH_OF_ARRAY(prefix_data));
  assert_true(!slice_int_starts_with(&slice, &prefix), "slice doesn't contain it");

  slice_int_destroy(&slice);
  assert_true(!slice_int_starts_with(&slice, &prefix), "slice is empty");

  prefix = slice_int_from_raw_parts(NULL, 0);
  assert_true(slice_int_starts_with(&slice, &prefix), "slice always contains empty prefix");
}
#endif

#define DEFINE_SLICE_ENDS_WITH(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
/** Return true when suffix is found at end of the slice. */ \
NN WUR MU SI bool SELFPREFIX##_ends_with(const SELFNAME * self, const SELFNAME * suffix) { \
  if(SELFPREFIX##_is_empty(suffix)) return true; \
  if(SELFPREFIX##_is_empty(self)) return false; \
  size_t len = SELFPREFIX##_len(suffix); \
  if(len > SELFPREFIX##_len(self)) return false; \
  for(size_t i = 0, j=SELFPREFIX##_len(self)-len; i < len; i++, j++) { \
      if(!TYPEPREFIX##_eq(SELFPREFIX##_get_unchecked_mut(self, j), SELFPREFIX##_get_unchecked_mut(suffix, i))) return false; \
  } \
  return true; \
}
#ifdef _CRUST_TESTS
DEFINE_SLICE_ENDS_WITH(Slice_int, slice_int, int, int)
it(slice_int_ends_with, "must return true when suffix matches ending of the slice") {
  int data[] = { 0, 1, 2, 3, 4 };
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));

  {
    int suffix_data[] = { 2, 3, 4 };
    Slice_int suffix = slice_int_from_raw_parts(suffix_data, LENGTH_OF_ARRAY(suffix_data));
    assert_true(slice_int_ends_with(&slice, &suffix), "slice contains it");
  }

  {
    int suffix_data[] = { 0, 1, 2, 3, 4 };
    Slice_int suffix = slice_int_from_raw_parts(suffix_data, LENGTH_OF_ARRAY(suffix_data));
    assert_true(slice_int_ends_with(&slice, &suffix), "slice contains it");
  }

  {
    Slice_int suffix = slice_int_default();
    assert_true(slice_int_ends_with(&slice, &suffix), "slice contains it");
  }

  {
    Slice_int suffix = slice_int_from_raw_parts(NULL, 0);
    assert_true(slice_int_ends_with(&slice, &suffix), "slice always contains empty suffix");
  }

  int suffix_data[] = { 1, 2 };
  Slice_int suffix = slice_int_from_raw_parts(suffix_data, LENGTH_OF_ARRAY(suffix_data));
  assert_true(!slice_int_ends_with(&slice, &suffix), "slice doesn't contain it");

  slice_int_destroy(&slice);
  assert_true(!slice_int_ends_with(&slice, &suffix), "slice is empty");

  suffix = slice_int_from_raw_parts(NULL, 0);
  assert_true(slice_int_ends_with(&slice, &suffix), "slice always contains empty suffix");
}
#endif

// TODO: first, last, shift, pop, rotate, first_mut, last_mut, binary_search, sort


// Define whole template
#define DEFINE_SLICE_BY_VALUE_TEMPLATE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
  DEFINE_SLICE_STRUCT(SELFNAME) \
  DEFINE_SLICE_FROM_RAW_PARTS(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_DESTROY(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_DEFAULT(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_LEN(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_CLONE(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_AS_PTR(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_GET(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_GET_UNCHECKED(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_GET_MUT(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_GET_UNCHECKED_MUT(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_GET_OR_DEFAULT(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_IS_EMPTY(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_SWAP(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_SLICE_REVERSE(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_SLICE(SELFNAME, SELFPREFIX) \
  DEFINE_SLICE_EQ(SELFNAME, SELFPREFIX, TYPEPREFIX) \
  DEFINE_SLICE_CONTAINS(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
  DEFINE_SLICE_STARTS_WITH(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
  DEFINE_SLICE_ENDS_WITH(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \

#endif /* CRUST_TYPE_SLICE_H_ */
