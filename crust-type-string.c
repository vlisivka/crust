#include "crust-type-string.h"

void string_panic(int error_code, const char * value) {
  switch(error_code) {
    case STRING_ERROR_BAD_FORMAT:
      fprintf(stderr, "ERROR: String: Bad format string: \"%s\".\n", value);
    break;

    default:
      fprintf(stderr, "ERROR: string_panic(): Unknown error code: %d.\n", error_code);
  }

  abort();
}

size_t string_printf(String * self, const char * fmt, ...) {
  _Vec * super = &self->super;
  va_list ap;

  // Determine required size
  va_start(ap, fmt);
  int size = vsnprintf(NULL, 0, fmt, ap);
  va_end(ap);

  if (size < 0) {
    string_panic(STRING_ERROR_BAD_FORMAT, fmt);
  }

  // Allocate space, including space for '\0'
  string_reserve(self, size+1);

  // Print into buffer
  va_start(ap, fmt);
  size = vsnprintf(&((char*)super->data)[super->count], size+1, fmt, ap);
  va_end(ap);

  if (size < 0) {
    string_panic(STRING_ERROR_BAD_FORMAT, fmt);
  }

  super->count += size;

  return size; // '\0' is not counted
}

size_t string_put_char(String * self, char value) {
  _Vec * super = &self->super;

  if (super->count+1 == super->capacity) {
    _vec_reserve(super, sizeof(char), 2);
  }

  char * data = string_as_ptr(self);
  data[super->count] = value;
  data[super->count+1] = '\0';

  return super->count++; // '\0' is not counted
}

size_t string_put_charp(String * self, const char * value) {
  _Vec * super = &self->super;

  if(!value) {
    // Do nothing when NULL is passed
    return super->count;
  }

  size_t length = strlen(value);

  if(length > 0) {
    _vec_reserve(super, sizeof(char), length+1);

    strncpy(&string_as_ptr(self)[super->count], value, length+1);
  }

  return super->count += length; // '\0' is not counted
}

size_t string_end_with_zero(String * self) {
  _Vec * super = &self->super;

  if (super->count == super->capacity) {
    _vec_reserve(super, sizeof(char), 1);
  }

  string_as_ptr(self)[super->count] = '\0';

  return super->count; // '\0' is not counted
}
