#include "crust-type-charp.h"

#include "crust-mem.h"
#include "crust-unittest.h"

it(charp_default, "must return empty string") {
   defer(charp_destroy) char * s = charp_default();
   assert_equal_int(0, strlen(s), "Unexpected length");
   assert_equal_int('\0', *s, "Unexpected value");
}

