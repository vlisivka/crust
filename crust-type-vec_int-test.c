// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.


#include "crust-type-vec.h"
#include "crust-type-int.h"
#include "crust-type-array.h"
#include "crust-type-option.h"
#include "crust-type-string.h"

#include "crust-unittest.h"

//
// Vec_int
//
VEC_BY_VALUE_TEMPLATE(Vec_int, vec_int, int)
DEFINE_SLICE_BY_VALUE_TEMPLATE(Slice_int, slice_int, int, int)
VEC_TO_SLICE(Vec_int, vec_int, int, Slice_int, slice_int)
DEFINE_OPTION_BY_VALUE(Option_int, option_int, int)

//DEFINE_vec_debug_print(Vec_int, vec_int, "%d, ")
//DEFINE_vec_debug_print(Slice_int, slice_int, "%d, ")
DEFINE_vec_debug(Vec_int, vec_int, "%d, ")
DEFINE_vec_debug(Slice_int, slice_int, "%d, ")

//

it(vec_new_push_get_len, "must create new vector, allow to push and get values, and it length must be correct") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();

  for(size_t i=0; i<17; i++) {
    vec_int_push(&vec, i);
  }

  assert_equal_int(17, vec_int_len(&vec), "Unexpected size of the vector after vec_int_push()");

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, vec_int_get(&vec, i), "Unexpected value of item after vec_int_push()");
  }
}

it(vec_int_destroy, "must destroy vector and clear it values, so double destroy is safe") {
  Vec_int vec = vec_int_new();
  vec_int_destroy(&vec);
  vec_int_destroy(&vec);
}

it(vec_capacity, "must return capacity of vector") {
  defer(vec_int_destroy) Vec_int vec = vec_int_with_capacity(111);

  assert_equal_int(111, vec_int_capacity(&vec), "Unexpected capacity in vector created by vec_int_with_capacity()");

}

it(vec_reserve_exact, "must extended vector capacity to given value") {
  defer(vec_int_destroy) Vec_int vec = vec_int_with_capacity(111);

  assert_equal_int(111, vec_int_capacity(&vec), "Unexpected capacity in vector created by vec_int_with_capacity()");

  vec_int_reserve_exact(&vec, 200);

  assert_equal_int(200, vec_int_capacity(&vec), "Unexpected capacity after vec_int_reserve_exact()");
}

it(vec_set, "must replace values in vector, with checking of bounds, and return replaced value") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();

  for(size_t i=0; i<17; i++) {
    vec_int_push(&vec, i);
  }

  assert_equal_int(17, vec_int_len(&vec), "Unexpected size of the vector after vec_int_push()");

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, vec_int_get(&vec, i), "Unexpected value of item after vec_int_push()");
  }

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    vec_int_set(&vec, i, i*i);
  }

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i*i, vec_int_get(&vec, i), "Unexpected value of item after vec_int_set()");
  }
}

it(vec_set__abort, "must abort on out of bounds") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();
  assert_abort(vec_int_set(&vec, 1, 42), "vec set() must call abort on index out of bounds");
}

it(vec_from_datap, "must create vector from raw pointer to data") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  assert_equal_int(11, vec_int_len(&vec), "Unexpected size of the vector after vec_int_push()");

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, vec_int_get(&vec, i), "Unexpected value of item after vec_int_from_raw_parts()");
  }
}

it(vec_get,"must return value from vector, with checking of bounds") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  assert_equal_int(11, vec_int_len(&vec), "Unexpected size of the vector after vec_int_from_datap()");

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, vec_int_get(&vec, i), "Unexpected value of item");
  }

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, vec_int_get_unchecked(&vec, i), "Unexpected value of item");
  }

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, *vec_int_get_mut(&vec, i), "Unexpected value of item");
  }

  for(size_t i=0; i<vec_int_len(&vec); i++) {
    assert_equal_int(i, *vec_int_get_unchecked_mut(&vec, i), "Unexpected value of item");
  }
}

it(vec_get_or_default, "must return value, or default value, when value is not found") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();

  assert_equal_int(12, vec_int_get_or_default(&vec, 1, 12), "Unexpected value of item");
}

it(vec_clone, "must return copy of vector") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  defer(vec_int_destroy) Vec_int vec1 = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));
  assert_equal_int(11, vec_int_len(&vec1), "Unexpected size of the vector after vec_int_from_datap()");
  defer(vec_int_destroy) Vec_int vec2 = vec_int_clone(vec1);
  assert_equal_int(11, vec_int_len(&vec2), "Unexpected size of the vector after vec_int_clone()");

  vec_int_set(&vec1, 0, 100);
  assert_equal_int(100, vec_int_get(&vec1, 0), "Value not changed after vec_int_set()");
  assert_equal_int(0, vec_int_get(&vec2, 0), "Value changed in clone of vector after change in original");
  assert_equal_int(0, data[0], "Value changed in original array after change in vector");
}

it(vec_int_from_raw_parts_unsafe, "must create vector from pointer to data without copying of the data (unsafe)") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  Vec_int vec1 = vec_int_from_raw_parts_unsafe(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));
  assert_equal_int(11, vec_int_len(&vec1), "Unexpected size of the vector after vec_int_from_raw_parts_unsafe()");
  assert_equal_int(11, vec_int_capacity(&vec1), "Unexpected capacity of the vector after vec_int_from_raw_parts_unsafe()");

  Vec_int vec2 = vec_int_from_raw_parts_unsafe(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  vec_int_set(&vec1,0, 100);
  assert_equal_int(100, vec_int_get(&vec1, 0), "Value not changed after vec_int_set()");
  assert_equal_int(100, vec_int_get(&vec2, 0), "Value not changed in shared array after vec_int_set() in other vector");
  assert_equal_int(100, data[0], "Value not changed in shared array after vec_int_set() in other vector");

  // Array is allocated on stack, so no need for call to destroy()
}

it(vec_int_reserve, "should extend vector capacity") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();

  vec_int_reserve(&vec, 200);

  assert_true(vec_int_capacity(&vec)>=200, "Capacity must be more or equal to 200 after vec_int_reserve(200)");
}

it(vec_int_shrink_to_fit, "must decrease capactiy to minimum amount") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), 100);

  assert_equal_int(100, vec_int_capacity(&vec), "Unexpected capacity of the vector after vec_int_from_datap()");

  vec_int_shrink_to_fit(&vec);

  assert_equal_int(11, vec_int_capacity(&vec), "Unexpected capacity of the vector after vec_int_shrink_to_fit()");
}

it(vec_int_truncate, "") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));
  assert_equal_int(11, vec_int_capacity(&vec), "Unexpected capacity of the vector after vec_int_shrink_to_fit()");

  vec_int_truncate(&vec, 5);

  assert_equal_int(5, vec_int_len(&vec), "Unexpected length of the vector after vec_int_truncate()");
  assert_equal_int(11, vec_int_capacity(&vec), "Unexpected capacity of the vector after vec_int_truncate()");
}

it(vec_int_as_slice, "") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), 100);

  Slice_int slice = vec_int_as_slice(&vec);

  assert_equal_int(10, slice_int_get(&slice, 10), "Unexpected value after vec_int_as_slice()");
}

it(vec_int_from_slice, "") {
  int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));

  defer(vec_int_destroy) Vec_int vec = vec_int_from_slice(&slice);

  assert_equal_int(10, vec_int_get(&vec, 10), "Unexpected value after vec_int_from_slice()");
}

static inline bool vec_int_is_empty(const Vec_int * self) {
  return vec_int_len(self) == 0;
}

static inline Option_int vec_int_first(const Vec_int * self) {
  if(vec_int_is_empty(self))
    return option_int_none();
  else
    return option_int_some(vec_int_get_unchecked(self, 0));
}

it(vec_int_first, "must return first element of vector on None") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();
  Option_int result = vec_int_first(&vec);
  assert_true(option_int_is_none(&result), "vec_int_first() must return None when vector is empty");

  vec_int_push(&vec, 42);
  assert_equal_int(42, option_int_unwrap(vec_int_first(&vec)), "vec_int_first() must return first element of vector");

  vec_int_push(&vec, 2);
  assert_equal_int(42, option_int_unwrap(vec_int_first(&vec)), "vec_int_first() must return first element of vector");
}

static inline Option_int vec_int_last(const Vec_int * self) {
  if(vec_int_is_empty(self))
    return option_int_none();
  else
  return option_int_some(vec_int_get_unchecked(self, vec_int_len(self)-1));
}

it(vec_int_last, "must return last element of vector on None") {
  defer(vec_int_destroy) Vec_int vec = vec_int_new();
  Option_int result = vec_int_last(&vec);
  assert_true(option_int_is_none(&result), "vec_int_last() must return None when vector is empty");

  vec_int_push(&vec, 1);
  assert_equal_int(1, option_int_unwrap(vec_int_last(&vec)), "vec_int_last() must return last element of vector");

  vec_int_push(&vec, 42);
  assert_equal_int(42, option_int_unwrap(vec_int_last(&vec)), "vec_int_last() must return last element of vector");
}

static inline int vec_int_swap_remove(Vec_int * self, size_t index) {
  int removed_element = vec_int_get(self, index);

  *vec_int_get_unchecked_mut(self,index) = vec_int_get(self, vec_int_len(self)-1);
  vec_int_set_len_unsafe(self, vec_int_len(self)-1);

  return removed_element;
}

it(vec_int_swap_remove, "must remove element, return it, and replace it with last element") {
  int data[] = { 0, 1, 2, 3, 4, 5 };
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  assert_equal_int(3, vec_int_swap_remove(&vec, 3), "Swap remove must return deleted element");
  assert_equal_int(5, vec_int_get(&vec, 3), "Swap remove must replace deleted element with last element");
  assert_equal_int(5, vec_int_len(&vec), "Swap remove must shrink vector after delete");

  assert_equal_int(4, vec_int_swap_remove(&vec, 4), "Swap remove must return deleted element");
  assert_equal_int(4, vec_int_len(&vec), "Swap remove must shrink vector after delete");
}

static inline int vec_int_contains(const Vec_int * self, int element) {
  if(vec_int_is_empty(self)) return false;

  for(size_t i=0; i < vec_int_len(self); i++) {
      if(vec_int_as_ptr(self)[i] == element) return true;
  }

  return false;
}

static inline bool vec_int_eq(const Vec_int * self, const Vec_int * other) {
  size_t self_len = vec_int_len(self);
  size_t other_len = vec_int_len(other);

  if(self_len != other_len) return false;

  for(size_t i=0; i < self_len; i++) {
      if(vec_int_get_unchecked(self, i) != vec_int_get_unchecked(other, i)) return false;
  }

  return true;
}

static inline void vec_int_clear(Vec_int * self) {
  vec_int_set_len_unsafe(self, 0);
}

it(vec_int_slice, "must create slice from vector") {
  int data[] = { 0, 1, 2, 3, 4, 5 };
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  Slice_int slice = vec_int_slice(&vec, 2, 3);
  assert_equal_int(3, slice_int_len(&slice), "Incorrect length");

  assert_equal_int(2, slice_int_get(&slice, 0), "Incorrect value");
  assert_equal_int(3, slice_int_get(&slice, 1), "Incorrect value");
  assert_equal_int(4, slice_int_get(&slice, 2), "Incorrect value");
}

/**
 * Removes element from vector at index, rest of elements are shifted to the left.
 * Capacity is not changed.
 *
 * @throws Panics if index is out of bounds.
 *
 * @return removed element
 */
static int vec_int_remove(Vec_int * self, int index) {
  int old_element = vec_int_get(self, index);

  size_t len = vec_int_len(self);
  size_t delta = len - index-1;
  if(delta > 0) {
    memmove(vec_int_get_unchecked_mut(self, index), vec_int_get_unchecked_mut(self, index+1), delta*sizeof(int));
  }
  vec_int_set_len_unsafe(self, len-1);

  return old_element;
}

it(vec_int_remove, "must remove and return element and shift other elements") {
  int data[] = { 0, 1, 2, 3, 4, 5 };
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  assert_equal_int(2, vec_int_remove(&vec, 2), "vec_int_remove() must return value of deleted element");
  assert_equal_int(5, vec_int_len(&vec), "vec_int_remove() must shift rest of elements to the left");
  assert_equal_int(3, vec_int_get(&vec, 2), "vec_int_remove() must shift rest of elements to the left");
  assert_equal_int(5, vec_int_get(&vec, 4), "vec_int_remove() must shift rest of elements to the left");

  assert_equal_int(5, vec_int_remove(&vec, 4), "vec_int_remove() must return value of deleted element");
  assert_equal_int(4, vec_int_len(&vec), "vec_int_remove() must shift rest of elements to the left");

  assert_equal_int(0, vec_int_remove(&vec, 0), "vec_int_remove() must return value of deleted element");
  assert_equal_int(3, vec_int_len(&vec), "vec_int_remove() must shift rest of elements to the left");
  assert_equal_int(4, vec_int_get(&vec, 2), "vec_int_remove() must shift rest of elements to the left");
}

/**
 * Inserts element into vector at index, elements after it are shifted to the right.
 * Capacity is increased if necessary.
 *
 * @throws Panics if index is out of bounds.
 */
static void vec_int_insert(Vec_int * self, size_t index, int value) {
  if(index > vec_int_len(self)) {
      _vec_panic(_VEC_ERROR_INDEX_OUT_OF_BOUNDS, index);
  }

  size_t len = vec_int_len(self);
  if(len >= vec_int_capacity(self)) {
      vec_int_reserve(self, 1);
  }

  size_t delta = len - index;

  if(delta > 0) {
    memmove(vec_int_get_unchecked_mut(self, index+1), vec_int_get_unchecked_mut(self, index), delta*sizeof(int));
  }
  vec_int_set_len_unsafe(self, len+1);
  *vec_int_get_unchecked_mut(self, index) = value;
}

it(vec_int_insert, "must insert element and shift other elements") {
  int data[] = { 0, 1, 2, 3 };
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));

  vec_int_insert(&vec, 2, 20);
  assert_equal_int(20, vec_int_get(&vec, 2), "vec_int_insert() must insert value at given position");
  assert_equal_int(5, vec_int_len(&vec), "vec_int_insert() must increase length of vector");
  assert_equal_int(3, vec_int_get(&vec, 4), "vec_int_insert() must shift rest of elements to the right");

  vec_int_insert(&vec, 5, 50);
  assert_equal_int(50, vec_int_get(&vec, 5), "vec_int_insert() must insert value at given position");
  assert_equal_int(6, vec_int_len(&vec), "vec_int_insert() must shift rest of elements to the right");

  vec_int_insert(&vec, 0, 10);
  assert_equal_int(10, vec_int_get(&vec, 0), "vec_int_insert() must insert value at given position");
  assert_equal_int(7, vec_int_len(&vec), "vec_int_insert() must shift rest of elements to the right");
}


it(vec_int_debug, "must return String builder with content of vector") {
  int data[] = {1, 2, 3, 4, 5};
  defer(vec_int_destroy) Vec_int vec = vec_int_from_datap(data, LENGTH_OF_ARRAY(data), LENGTH_OF_ARRAY(data));
  defer(string_destroy) String str = vec_int_debug(&vec);

  assert_equal_charp("Vec_int={1, 2, 3, 4, 5, }", string_as_ptr(&str), "vec_int_debug() must return content of vector");
}

it(slice_int_debug, "must return String builder with content of slice") {
  int data[] = {1, 2, 3, 4, 5};
  Slice_int slice = slice_int_from_raw_parts(data, LENGTH_OF_ARRAY(data));
  defer(string_destroy) String str = slice_int_debug(&slice);

  assert_equal_charp("Slice_int={1, 2, 3, 4, 5, }", string_as_ptr(&str), "slice_int_debug() must return content of slice");
}
