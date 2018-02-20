
#include "crust-type-charp.h"
#include "crust-type-ccharp.h"
#include "crust-type-vec.h"

#include "crust-unittest.h"

VEC_BY_VALUE_TEMPLATE(Vec_ccharp, vec_ccharp, const char *) // @suppress("Unused static function")
DEFINE_SLICE_BY_VALUE_TEMPLATE(Slice_ccharp, slice_ccharp, const char *, ccharp) // @suppress("Unused static function")
VEC_TO_SLICE(Vec_ccharp, vec_ccharp, const char *, Slice_ccharp, slice_ccharp) // @suppress("Unused static function")

it(ccharp, "must work correctly with the vector type") {
  int argc=3;
  const char * argv[] = { "arg1", "arg2", "arg3" };
  defer(vec_ccharp_destroy) Vec_ccharp vec = vec_ccharp_from_datap(argv, argc, 4); // Pointers to strings are copied

  assert_equal_charp("arg1", vec_ccharp_get(&vec, 0), "Unexpected value");
  assert_equal_charp("arg2", vec_ccharp_get(&vec, 1), "Unexpected value");
  assert_equal_charp("arg3", vec_ccharp_get(&vec, 2), "Unexpected value");

  const char * val = "arg4";
  vec_ccharp_push(&vec, val);

  assert_equal_charp("arg4", vec_ccharp_get(&vec, 3), "Unexpected value");
}
