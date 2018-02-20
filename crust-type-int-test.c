#include "crust-type-int.h"
#include "crust-unittest.h"

it(int_default, "must return 0") {
  assert_equal_int(0, int_default(), "Unexpected value");
}

