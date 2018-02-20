#ifndef CRUST_STRING_H
#define CRUST_STRING_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "crust-mem.h"

#include "crust-type-vec.h"
#include "crust-type-char.h"

VEC_BY_VALUE_TEMPLATE(String, string, char)
DEFINE_SLICE_BY_VALUE_TEMPLATE(Str, str, char, char)
VEC_TO_SLICE(String, string, char, Str, str)

enum String_error_codes {
  STRING_ERROR_BAD_FORMAT = 1,
};

static inline String string_default() {
  return string_with_capacity(1);
}

static inline String string_from_charp(const char *const charp) {
  size_t length = strlen(charp);
  return string_from_datap(charp, length, length+1);
}

static inline String string_from_str(const Str * str) {
  size_t length = str_len(str);
  return string_from_datap(str_as_ptr(str), length, length+1);
}


__attribute__ ((format (printf, 2, 3)))
size_t string_printf(String * self, const char * fmt, ...);

size_t string_put_char(String * self, char value);
size_t string_end_with_zero(String * self);
size_t string_put_charp(String * self, const char * value);
size_t string_put_str(String * self, const Str * value);

static inline Str str_from_charp(const char * charp) { return (Str) { .super = _slice_from_raw_parts( charp, strlen(charp) ) }; }
static inline Str str_from_string(const String *string) { return (Str) { .super = _slice_from_raw_parts( string_as_ptr(string), string_len(string) ) }; }

#endif
