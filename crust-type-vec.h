// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_VEC_H
#define CRUST_VEC_H

#include <sys/types.h>
#include <stdlib.h>
#include "crust-mem.h"

#include "crust-type-slice.h"

#ifdef _CRUST_TESTS
#include "crust-unittest.h"

#include "crust-type-int.h"
#include "crust-type-array.h"
#include "crust-type-option.h"
#endif

enum _Vec_error_codes {
  _VEC_ERROR_NO_DATA = 1,
  _VEC_ERROR_INDEX_OUT_OF_BOUNDS = 2,
  _VEC_ERROR_CAPACITY_TOO_SMALL = 3,
};

void _vec_panic(int error_code, size_t index);
#ifdef _CRUST_TESTS
it(_vec_panic, "must abort program using abort()") {
  assert_abort(_vec_panic(_VEC_ERROR_NO_DATA, 1), "must abort");
  assert_abort(_vec_panic(_VEC_ERROR_INDEX_OUT_OF_BOUNDS, 1), "must abort");
  assert_abort(_vec_panic(_VEC_ERROR_CAPACITY_TOO_SMALL, 1), "must abort");
  assert_abort(_vec_panic(12312, 1), "must abort");
}
#endif


typedef struct _Vec_s {
  void * data;
  size_t count;
  size_t capacity;
} _Vec;

/** Free data pointer and clear pointer, length, and capacity.
 * It's safe to call destroy() twice.
 * It's safe to use vector again after destroy. */
NN void _vec_destroy(_Vec * self);

/** Create new empty vector with given capacity.
 * It's recommended to create vectors with this method. */
WUR struct _Vec_s _vec_with_capacity(size_t element_size, size_t capacity);
#ifdef _CRUST_TESTS
it(_vec_with_capacity, "must create new vector with given capacity") {
  defer(_vec_destroy) _Vec vec = _vec_with_capacity(sizeof(int), 4);
  assert_equal_int(0, vec.count, "unexpected length");
  assert_equal_int(4, vec.capacity, "unexpected capacity");
}
#endif

/** Create new vector by copying given data into vector.
 * Members of array are copied, not cloned.
 * Panics when capacity is less than length of array.
 * Panics when pointer is NULL but length or capacity is not 0. */
WUR struct _Vec_s _vec_from_datap(size_t element_size, const void * data, size_t length, size_t capacity);
#ifdef _CRUST_TESTS
it(_vec_from_datap, "must create new vector with given capacity and shallow copy of data") {
  int data[32] = {0, 1, 2, 3, 4};
  defer(_vec_destroy) _Vec vec = _vec_from_datap(sizeof(int), data, 5, LENGTH_OF_ARRAY(data));
  assert_equal_int(5, vec.count, "unexpected length");
  assert_equal_int(32, vec.capacity, "unexpected capacity");
  ((int *)vec.data)[0]=42;
  assert_equal_int(0, data[0], "must create copy of data");
}
#endif

/** Create new vector using existing array with given capacity.
 * No new data is allocated.
 * Panics when capacity is less than length of array.
 * Panics when pointer is NULL but length or capacity is not 0. */
WUR struct _Vec_s _vec_from_raw_parts_unsafe(void * data, size_t length, size_t capacity);
#ifdef _CRUST_TESTS
it(_vec_from_raw_parts_unsafe, "must create new vector with given capacity and no copy of data") {
  int data[32] = {0, 1, 2, 3, 4};
  _Vec vec = _vec_from_raw_parts_unsafe(data, 5, LENGTH_OF_ARRAY(data));
  assert_equal_int(5, vec.count, "unexpected length");
  assert_equal_int(32, vec.capacity, "unexpected capacity");
  ((int *)vec.data)[0]=42;
  assert_equal_int(42, data[0], "must use existing array");
}
#endif

/** Create new vector with default capacity of 8 elements. */
WUR MU SI struct _Vec_s _vec_new(size_t element_size) { return _vec_with_capacity(element_size, 8); }
#ifdef _CRUST_TESTS
it(_vec_new, "must create new empty vector with non-zero capacity") {
  defer(_vec_destroy) _Vec vec = _vec_new(sizeof(int));
  assert_equal_int(0, vec.count, "unexpected length");
  assert_equal_int(8, vec.capacity, "unexpected capacity");
}
#endif

/** Reallocate and resize array, if necessary, to hold additional capacity.
 * Capacity will be equal to or greater than length+additional_capacity. */
NN void _vec_reserve_exact(_Vec * self, size_t element_size, size_t additional_capacity);
#ifdef _CRUST_TESTS
it(_vec_reserve_exact, "must resize vector to hold additional elements") {
  defer(_vec_destroy) _Vec vec = _vec_new(sizeof(int));
  assert_equal_int(8, vec.capacity, "unexpected capacity after new()");
  _vec_reserve_exact(&vec, sizeof(int), 8);
  assert_equal_int(8, vec.capacity, "capacity must stay same");
  _vec_reserve_exact(&vec, sizeof(int), 10);
  assert_equal_int(10, vec.capacity, "capacity must increase");
}
#endif

/** Reallocate and resize array, if necessary, to hold additional capacity.
 * Capacity will be equal to or greater than length+additional_capacity.
 * May reserve additional capacity for better use of memory. */
NN void _vec_reserve(_Vec * self, size_t element_size, size_t additional_capacity);
#ifdef _CRUST_TESTS
it(_vec_reserve, "must resize vector to hold additional elements") {
  defer(_vec_destroy) _Vec vec = _vec_new(sizeof(int));
  assert_equal_int(8, vec.capacity, "unexpected capacity after new()");
  _vec_reserve(&vec, sizeof(int), 8);
  assert_equal_int(8, vec.capacity, "capacity must stay same");
  _vec_reserve(&vec, sizeof(int), 10);
  assert_true(10 <= vec.capacity, "capacity must be increased");
}
#endif

/** Resize, maybe reallocate, array, if necessary, to hold it current content only. */
NN void _vec_shrink_to_fit(_Vec * self, size_t element_size);
#ifdef _CRUST_TESTS
it(_vec_shrink_to_fit, "must resize vector to existing elements only") {
  int data[] = { 0, 1, 2, 4 };
  defer(_vec_destroy) _Vec vec = _vec_from_datap(sizeof(int), data, LENGTH_OF_ARRAY(data), 32);
  assert_equal_int(32, vec.capacity, "unexpected capacity after _vec_from_datap()");
  _vec_shrink_to_fit(&vec, sizeof(int));
  assert_equal_int(LENGTH_OF_ARRAY(data), vec.capacity, "capacity must be shrink");
}
#endif

/** Return capacity of the vector. */
NN WUR MU SI size_t _vec_capacity(const _Vec * self) { return self->capacity; }
#ifdef _CRUST_TESTS
it(_vec_capacity, "must return capacity of vector") {
  defer(_vec_destroy) _Vec vec = _vec_with_capacity(sizeof(int), 3);
  assert_equal_int(3, _vec_capacity(&vec), "unexpected capacity");
}
#endif

/** Return length of the vector. */
NN WUR MU SI size_t _vec_len(const _Vec * self) { return self->count; }
#ifdef _CRUST_TESTS
it(_vec_len, "must return capacity of vector") {
  int data[] = {0, 1, 2, 3, 4};
  defer(_vec_destroy) _Vec vec = _vec_from_datap(sizeof(int), data, LENGTH_OF_ARRAY(data), 32);
  assert_equal_int(5, _vec_len(&vec), "unexpected length");
}
#endif

//
// Template for Vec
//

#define DEFINE_VEC_STRUCT(SELFNAME) \
typedef struct { \
  _Vec super; \
} SELFNAME;

// Common functions

#define DEFINE_VEC_WITH_CAPACITY(SELFNAME, SELFPREFIX, CTYPE) \
WUR MU SI SELFNAME SELFPREFIX##_with_capacity(size_t capacity) { return (SELFNAME) { .super = _vec_with_capacity(sizeof(CTYPE), capacity) }; }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_NEW(SELFNAME, SELFPREFIX, CTYPE) \
WUR MU SI SELFNAME SELFPREFIX##_new() { return (SELFNAME) { .super = _vec_new(sizeof(CTYPE)) }; }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_FROM_RAW_PARTS_UNSAFE(SELFNAME, SELFPREFIX, CTYPE) \
WUR MU SI SELFNAME SELFPREFIX##_from_raw_parts_unsafe(CTYPE * data, size_t length, size_t capacity) { return (SELFNAME) { .super = _vec_from_raw_parts_unsafe(data, length, capacity) }; }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_RESERVE_EXACT(SELFNAME, SELFPREFIX, CTYPE) \
NN MU SI void SELFPREFIX##_reserve_exact(SELFNAME * self, size_t additional_capacity) {  _vec_reserve_exact(&self->super, sizeof(CTYPE), additional_capacity);}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_RESERVE(SELFNAME, SELFPREFIX, CTYPE) \
NN MU SI void SELFPREFIX##_reserve(SELFNAME * self, size_t additional_capacity) { _vec_reserve(&self->super, sizeof(CTYPE), additional_capacity); }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_SHRINK_TO_FIT(SELFNAME, SELFPREFIX, CTYPE) \
NN MU SI void SELFPREFIX##_shrink_to_fit(SELFNAME * self) { _vec_shrink_to_fit(&self->super, sizeof(CTYPE)); }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_CAPACITY(SELFNAME, SELFPREFIX) \
NN WUR MU SI size_t SELFPREFIX##_capacity(const SELFNAME * self) { return _vec_capacity(&self->super); }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_LEN(SELFNAME, SELFPREFIX) \
NN WUR MU SI size_t SELFPREFIX##_len(const SELFNAME * self) { return _vec_len(&self->super); }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_AS_PTR_UNSAFE(SELFNAME, SELFPREFIX, CTYPE) \
NN WUR MU SI CTYPE * SELFPREFIX##_as_ptr(const SELFNAME * self) { \
  const _Vec * super = &self->super; \
  return (CTYPE *) super->data; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_GET(SELFNAME, SELFPREFIX, CTYPE) \
NN WUR MU SI CTYPE SELFPREFIX##_get(const SELFNAME * self, size_t index) { \
  const _Vec * super = &self->super; \
 \
  if(index >= super->count) { \
    _vec_panic(_VEC_ERROR_INDEX_OUT_OF_BOUNDS, index); \
  } \
 \
  return SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_GET_MUT(SELFNAME, SELFPREFIX, CTYPE) \
NN WUR MU SI CTYPE * SELFPREFIX##_get_mut(const SELFNAME * self, size_t index) { \
  const _Vec * super = &self->super; \
 \
  if(index >= super->count) { \
    _vec_panic(_VEC_ERROR_INDEX_OUT_OF_BOUNDS, index); \
  } \
 \
  return &SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_(SELFNAME, SELFPREFIX, CTYPE) \
NN WUR MU SI CTYPE SELFPREFIX##_get_unchecked(const SELFNAME * self, size_t index) { \
  return SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_GET_UNCHECKED_MUT(SELFNAME, SELFPREFIX, CTYPE) \
NN WUR MU SI CTYPE * SELFPREFIX##_get_unchecked_mut(const SELFNAME * self, size_t index) { \
  return &SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_GET_OR_DEFAULT(SELFNAME, SELFPREFIX, CTYPE) \
NN WUR MU SI CTYPE SELFPREFIX##_get_or_default(const SELFNAME * self, size_t index, CTYPE default_value) { \
  const _Vec * super = &self->super; \
  if(index >= super->count) { \
    return default_value; \
  } \
  return SELFPREFIX##_as_ptr(self)[index]; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_SET_LEN_UNSAFE(SELFNAME, SELFPREFIX, CTYPE) \
NN MU SI void SELFPREFIX##_set_len_unsafe(SELFNAME * self, size_t length) { \
  _Vec * super = &self->super; \
  super->count = length; \
}
#ifdef _CRUST_TESTS
#endif

#define _VEC_COMMON(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_STRUCT(SELFNAME) \
DEFINE_VEC_WITH_CAPACITY(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_NEW(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_FROM_RAW_PARTS_UNSAFE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_RESERVE_EXACT(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_RESERVE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_SHRINK_TO_FIT(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_CAPACITY(SELFNAME, SELFPREFIX) \
DEFINE_VEC_LEN(SELFNAME, SELFPREFIX) \
DEFINE_VEC_AS_PTR_UNSAFE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_GET(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_GET_MUT(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_GET_UNCHECKED_MUT(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_GET_OR_DEFAULT(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_SET_LEN_UNSAFE(SELFNAME, SELFPREFIX, CTYPE) \

// By value

#define DEFINE_VEC_PUSH_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
NN MU SI size_t SELFPREFIX##_push(SELFNAME * self, const CTYPE value) { \
  _Vec * super = &self->super; \
  \
  if (super->count == super->capacity) { \
    _vec_reserve(super, sizeof(CTYPE), 1); \
  } \
 \
  SELFPREFIX##_as_ptr(self)[super->count] = value; \
 \
  return super->count++; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_FROM_DATAP_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
WUR MU SI SELFNAME SELFPREFIX##_from_datap(const CTYPE * data, size_t length, size_t capacity) { return (SELFNAME) { .super = _vec_from_datap(sizeof(CTYPE), data, length, capacity) }; }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_CLONE_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
WUR MU SI SELFNAME SELFPREFIX##_clone(SELFNAME other) { \
  const _Vec * super = &other.super; \
  return SELFPREFIX##_from_datap(super->data, super->count, super->count); \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_DESTROY_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
MU SI void SELFPREFIX##_destroy(SELFNAME * self) { _vec_destroy(&self->super); }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_TRUNCATE_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
MU SI void SELFPREFIX##_truncate(SELFNAME * self, size_t length) { SELFPREFIX##_set_len_unsafe(self, length); }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_SET_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
MU SI CTYPE SELFPREFIX##_set(SELFNAME * self, size_t index, const CTYPE value) { \
  CTYPE prev = SELFPREFIX##_get(self, index); \
  SELFPREFIX##_as_ptr(self)[index] = value; \
  return prev; \
}
#ifdef _CRUST_TESTS
#endif

#define VEC_BY_VALUE_TEMPLATE(SELFNAME, SELFPREFIX, CTYPE) \
_VEC_COMMON(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_PUSH_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_FROM_DATAP_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_CLONE_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_DESTROY_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_TRUNCATE_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_SET_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \

// By reference
// Requires TYPEPREFIX##_clone(CTYPE * e) and TYPEPREFIX##_destroy(CTYPE * e)

#define DEFINE_VEC_PUSH_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
MU SI size_t SELFPREFIX##_push_shallow(SELFNAME * self, CTYPE value) { \
  _Vec * super = &self->super; \
  \
  if (super->count == super->capacity) { \
    _vec_reserve(super, sizeof(CTYPE), 1); \
  } \
 \
  SELFPREFIX##_as_ptr(self)[super->count] = value; \
 \
  return super->count++; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_PUSH_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
MU SI size_t SELFPREFIX##_push(SELFNAME * self, const CTYPE value) { \
  return SELFPREFIX##_push_shallow(self, TYPEPREFIX##_clone(value)); \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_FROM_DATAP_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
WUR MU SI SELFNAME SELFPREFIX##_from_datap(const CTYPE * data, size_t length, size_t capacity) { \
  SELFNAME self={ .super = _vec_with_capacity(sizeof(CTYPE), capacity) }; \
  for(size_t i=0; i<length; i++) { \
    SELFPREFIX##_push(&self, data[i]); \
  } \
  return self; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_DATAP_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
WUR MU SI SELFNAME SELFPREFIX##_from_datap_shallow(const CTYPE * data, size_t length, size_t capacity) { return (SELFNAME) { .super = _vec_from_datap(sizeof(CTYPE), data, length, capacity) }; }
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_CLONE_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
WUR MU SI SELFNAME SELFPREFIX##_clone(SELFNAME other) { \
  const _Vec * super = &other.super; \
 \
  return SELFPREFIX##_from_datap(super->data, super->count, super->count); \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_CLONE_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
WUR MU SI SELFNAME SELFPREFIX##_clone_shallow(SELFNAME other) { \
  const _Vec * super = &other.super; \
 \
  return SELFPREFIX##_from_datap_shallow(super->data, super->count, super->count); \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_DESTROY_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
MU SI void SELFPREFIX##_destroy(SELFNAME * self) { \
  _Vec * super = &self->super; \
  for(size_t i=0; i < super->count; i++) { \
    TYPEPREFIX##_destroy(SELFPREFIX##_get_ref_unsafe(self, i)); \
  } \
  _vec_destroy(super); \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_TRUNCATE_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
MU SI void SELFPREFIX##_truncate(SELFNAME * self, size_t length) { \
  _Vec * super = &self->super; \
  if(length < super->count) { \
    for(; super->count > length; super->count--) { \
      TYPEPREFIX##_destroy(SELFPREFIX##_get_ref_unsafe(self, super->count)); \
    } \
  } \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_SET_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
MU SI CTYPE SELFPREFIX##_set_shallow(SELFNAME * self, size_t index, CTYPE value) { \
  CTYPE prev = SELFPREFIX##_get(self, index); \
  SELFPREFIX##_as_ptr(self)[index] = value; \
\
  return prev; \
}
#ifdef _CRUST_TESTS
#endif

#define DEFINE_VEC_SET_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
MU SI CTYPE SELFPREFIX##_set(SELFNAME * self, size_t index, const CTYPE value) { \
  return SELFPREFIX##_set_shallow(self, index, TYPEPREFIX##_clone(value)); \
}
#ifdef _CRUST_TESTS
#endif

#define VEC_BY_REF_TEMPLATE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
_VEC_COMMON(SELFNAME, SELFPREFIX, CTYPE) \
DEFINE_VEC_PUSH_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_PUSH_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_FROM_DATAP_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_DATAP_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_DESTROY_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_TRUNCATE_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_SET_SHALLOW_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
DEFINE_VEC_SET_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \

#define VEC_TO_SLICE(SELFNAME, SELFPREFIX, CTYPE, SLICETYPENAME, SLICEPREFIX) \
\
NN WUR MU SI SELFNAME SELFPREFIX##_from_slice(const SLICETYPENAME * other) { \
  return SELFPREFIX##_from_datap((const CTYPE *)SLICEPREFIX##_as_ptr(other), SLICEPREFIX##_len(other), SLICEPREFIX##_len(other)); \
} \
\
NN WUR MU SI SLICETYPENAME SELFPREFIX##_as_slice(const SELFNAME * self) { \
  const _Vec * super = &self->super; \
  return SLICEPREFIX##_from_raw_parts(super->data, super->count); \
} \
  \
/** Create slice from vector. No data is copied. */ \
NN WUR MU SI SLICETYPENAME SELFPREFIX##_slice(const SELFNAME * self, size_t start, size_t length) { \
  if(start+length > SELFPREFIX##_len(self)) { \
      _vec_panic(_VEC_ERROR_INDEX_OUT_OF_BOUNDS, start+length); \
  } \
  return SLICEPREFIX##_from_raw_parts(SELFPREFIX##_get_mut(self, start), length); \
}

/**
 * Define function to quickly print content of vector (or slice) to stdout for debugging purposes.
 */
#define DEFINE_vec_debug_print(SELFNAME, SELFPREFIX, PRINTFFMT) \
NN MU SI void SELFPREFIX##_debug_print(SELFNAME * self) { \
  printf(#SELFNAME "={"); \
  for(size_t i=0; i<SELFPREFIX##_len(self); i++) { \
      printf(PRINTFFMT, SELFPREFIX##_get(self, i)); \
  } \
  printf("}\n"); \
}


/**
 * Define function to show content of vector (or slice) as String for debugging purposes.
 */
#define DEFINE_vec_debug(SELFNAME, SELFPREFIX, PRINTFFMT) \
NN MU SI String SELFPREFIX##_debug(SELFNAME * self) { \
  String str = string_with_capacity(12+SELFPREFIX##_len(self)*3); \
  string_printf(&str, #SELFNAME "={"); \
  for(size_t i=0; i<SELFPREFIX##_len(self); i++) { \
      string_printf(&str, PRINTFFMT, SELFPREFIX##_get(self, i)); \
  } \
  string_printf(&str, "}"); \
  return str; \
}

#endif
