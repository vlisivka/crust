// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_TYPE_CCHARP_H_
#define CRUST_TYPE_CCHARP_H_

#include <string.h>
#include <stdbool.h>

#include "crust-mem.h"

//
// Type for preallocated strings
//

/** Return pointer to statically allocated string. Pointer is always same. */
WUR MU SI const char * ccharp_default() {
  // Constant strings are allocated in separate section, not on the stack
  return "";
}

/** Compare two strings using strcmp(). */
NN WUR MU SI bool ccharp_eq(const char * * left, const char * * right) {
  return *left == *right || strcmp(*left, *right) == 0;
}

/** Do nothing. */
NN MU SI void ccharp_destroy(const char * * value) { (void) value; }

/** Return same pointer. */
NN WUR MU SI const char * ccharp_clone(const char * other) {
  return other;
}

#endif /* CRUST_TYPE_CCHARP_H_ */
