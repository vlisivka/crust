#include "crust-type-char.h"
#include "crust-unittest.h"

it(char_default, "must return '\\0'") {
  assert_equal_int('\0', char_default(), "Unexpected value");
}

