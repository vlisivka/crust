// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "crust-type-vec.h"
#include "crust-mem.h"

void _vec_panic(int error_code, size_t value) {
  switch(error_code) {
    case _VEC_ERROR_NO_DATA:
      fprintf(stderr, "ERROR: Vec: Pointer to data is NULL but length is not 0. Length: %zu.\n", value);
    break;

    case _VEC_ERROR_CAPACITY_TOO_SMALL:
      fprintf(stderr, "ERROR: Vec: Vector capacity is too small to hold vector data. Capacity: %zu.\n", value);
    break;

    case _VEC_ERROR_INDEX_OUT_OF_BOUNDS:
      fprintf(stderr, "ERROR: Vec: Index is out of bound. Index: %zu.\n", value);
    break;

    default:
      fprintf(stderr, "ERROR: _vec_panic(): Unknown error code: %d.\n", error_code);
  }

  abort();
}

_Vec _vec_with_capacity(size_t element_size, size_t capacity) {
  _Vec self = { .count = 0, .capacity = capacity, .data = mem_calloc(capacity, element_size) };

  return self;
}

_Vec _vec_from_datap(size_t element_size, const void * data, size_t length, size_t capacity) {
  if(!data && ( length > 0 || capacity >0) ) {
    _vec_panic(_VEC_ERROR_NO_DATA, length);
  }

  if(capacity < length) {
    _vec_panic(_VEC_ERROR_CAPACITY_TOO_SMALL, capacity);
  }

  _Vec self = _vec_with_capacity(element_size, capacity);

  if(data) {
    memcpy(self.data, data, length * element_size);
  }
  self.count = length;

  return self;
}

_Vec _vec_from_raw_parts_unsafe(void * data, size_t length, size_t capacity) {
  if(!data && ( length > 0 || capacity >0) ) {
    _vec_panic(_VEC_ERROR_NO_DATA, length);
  }

  if(capacity < length) {
    _vec_panic(_VEC_ERROR_CAPACITY_TOO_SMALL, capacity);
  }

  _Vec self = { .count = length, .capacity = capacity, .data = data };

  return self;
}

void _vec_reserve_exact(_Vec * self, size_t element_size, size_t additional_capacity) {
  size_t new_capacity = self->count + additional_capacity;

  if(new_capacity < self->count || new_capacity < additional_capacity) {
    mem_panic(MEM_ERROR_INTEGER_OVERFLOW, 0);
  }

  if(new_capacity > self->capacity) {
    self->data = mem_realloc(self->data, new_capacity, element_size);
    self->capacity = new_capacity;
  }
}

void _vec_reserve(_Vec * self, size_t element_size, size_t additional_capacity) {
  size_t delta = 4;

  if(additional_capacity > delta) {
    delta = additional_capacity;
  }

  if(self->capacity/3 > delta) {
    delta = self->capacity/3;
  }

  _vec_reserve_exact(self, element_size, delta);
}

void _vec_shrink_to_fit(_Vec * self, size_t element_size) {
  size_t new_capacity = self->count;

  self->data = mem_realloc(self->data, new_capacity, element_size);
  self->capacity = new_capacity;
}

void _vec_destroy(_Vec * self) {
  if(self->data) {
    free(self->data);
    self->data = NULL;
    self->count = 0;
    self->capacity = 0;
  }
}
