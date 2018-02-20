#include "crust-type-size_t.h"
#include "crust-unittest.h"

it(sizet_default, "must return 0") {
  assert_equal_int(0, sizet_default(), "Unexpected value");
}

