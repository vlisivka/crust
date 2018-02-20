// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_TYPE_INT_H_
#define CRUST_TYPE_INT_H_

#include <stdbool.h>

#include "crust-mem.h"

WUR MU SI int int_default() {
  return 0;
}

NN WUR MU SI bool int_eq(const int * left, const int * right) {
  return *left == *right;
}

#endif /* CRUST_TYPE_INT_H_ */
