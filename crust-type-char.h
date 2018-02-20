// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_TYPE_CHAR_H_
#define CRUST_TYPE_CHAR_H_

#include <stdbool.h>

static inline char char_default() {
  return '\0';
}

static inline bool char_eq(const char * left, const char * right) {
  return *left == *right;
}

#endif /* CRUST_TYPE_CHAR_H_ */
