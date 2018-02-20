#include <stdio.h>
#include <stdint.h>

#include "crust-mem.h"

void mem_panic(int error_code, size_t value) {
  switch(error_code) {
    case MEM_ERROR_OUT_OF_MEM:
      fprintf(stderr, "ERROR: Mem: Out of memory.");
      fprintf(stderr, "ERROR: Mem: Cannot allocate %zu bytes.\n", value);
    break;

    // FIXME: move to math.h
    case MEM_ERROR_INTEGER_OVERFLOW:
      fprintf(stderr, "ERROR: Integer overflow.\n");
    break;

    default:
      fprintf(stderr, "ERROR: mem_panic(): Unknown error code: %d.\n", error_code);
  }

  abort();
}

void * mem_realloc(void *ptr, size_t length, size_t element_size) {
  if(SIZE_MAX/element_size < length) {
    mem_panic(MEM_ERROR_INTEGER_OVERFLOW, 0);
  }

  void * result = realloc(ptr, length * element_size);

  if(!result && length > 0) {
    mem_panic(MEM_ERROR_OUT_OF_MEM, 0);
  }

  return result;
}

void * mem_calloc(size_t length, size_t element_size) {
  if(SIZE_MAX/element_size < length) {
    mem_panic(MEM_ERROR_INTEGER_OVERFLOW, 0);
  }

  void * result = calloc(length, element_size);

  if(!result && length > 0) {
    mem_panic(MEM_ERROR_OUT_OF_MEM, 0);
  }

  return result;
}
