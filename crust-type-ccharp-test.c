#include "crust-type-ccharp.h"

#include "crust-mem.h"
#include "crust-unittest.h"


it(ccharp_default, "must return empty static string") {
  const char * s = ccharp_default();
  assert_equal_int(0, *s, "Unexpected value");
}

