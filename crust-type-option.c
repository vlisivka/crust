/*
 * option.c
 *
 *  Created on: 28 січ. 2018 р.
 *      Author: vlisivka
 */
#include <stdio.h>
#include <stdlib.h>
#include "crust-type-option.h"

void option_panic(int error_code, const char * value) {
  switch(error_code) {
    case OPTION_ERROR_VALUE_EXPECTED:
      fprintf(stderr, "ERROR: Option: Value expected: \"%s\".\n", value);
    break;

    default:
      fprintf(stderr, "ERROR: option_panic(): Unknown error code: %d.\n", error_code);
  }

  abort();
}

