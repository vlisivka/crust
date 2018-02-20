// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_TYPE_CHARP_H_
#define CRUST_TYPE_CHARP_H_

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "crust-mem.h"

//
// Dynamically allocated C strings
//

/** Free memory allocated for string. */
NN MU SI void charp_destroy(char * * value) {
  if(*value) {
    free(*value);
    *value = NULL;
  }
}

/** Return pointer to allocated empty string. */
WUR MU SI char * charp_default() {
  return calloc(1, sizeof(char));
}

/** Compare two strings using strcmp(). */
NN WUR MU SI bool charp_eq(const char * * left, const char * * right) {
  return *left == *right || strcmp(*left, *right) == 0;
}

/** Allocate memory and copy string into it. */
NN WUR MU SI char * charp_clone(const char * other) {
  size_t len = strlen(other);
  char * self = mem_malloc(len+1, sizeof(char));
  strncpy(self, other, len+1);

  return self;
}

#endif /* CRUST_TYPE_CHARP_H_ */
