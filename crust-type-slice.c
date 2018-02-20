#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "crust-type-slice.h"
#include "crust-mem.h"

void _slice_panic(int error_code, size_t value) {
  switch(error_code) {
    case _SLICE_ERROR_NO_DATA:
      fprintf(stderr, "ERROR: Slice: Pointer to data is NULL but length is not 0. Length: %zu.\n", value);
    break;

    case _SLICE_ERROR_INDEX_OUT_OF_BOUNDS:
      fprintf(stderr, "ERROR: Slice: Index is out of bound. Index: %zu.\n", value);
    break;

    default:
      fprintf(stderr, "ERROR: _slice_panic(): Unknown error code: %d.\n", error_code);
  }

  abort();
}
