// Copyright 2018 Volodymyr M. Lisivka <vlisivka@gmail.com>.
// See the COPYRIGHT file at the top directory of this project.
//
// Licensed under the GPL License, Version 3.0 or later, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

#ifndef CRUST_OPTION_H_
#define CRUST_OPTION_H_

#include <stdbool.h>
#include "crust-mem.h"

#ifdef _CRUST_TESTS
#include "crust-unittest.h"
#endif

enum Option_error_codes {
  OPTION_ERROR_VALUE_EXPECTED,
};

void option_panic(int error_code, const char * value);
#ifdef _CRUST_TESTS
it(option_panic, "must print error message and abort program") {
  assert_abort(option_panic(OPTION_ERROR_VALUE_EXPECTED, "a message"), "must panic");
}
#endif

//
// Option by value
//

#define DEFINE_OPTION_STRUCT_BY_VALUE(SELFNAME, CTYPE) \
/** Option for types without neutral value, such as NULL, so they require separate boolean flag. */ \
typedef struct SELFNAME##_s { \
  bool has_value; \
  CTYPE value; \
} SELFNAME;
#ifdef _CRUST_TESTS
DEFINE_OPTION_STRUCT_BY_VALUE(Option_int, int)
#endif

#define DEFINE_OPTION_IS_NONE_BY_VALUE(SELFNAME, SELFPREFIX) \
/** Return true when None, or false otherwise. */ \
NN WUR MU SI bool SELFPREFIX##_is_none(const SELFNAME * self) { \
  return !self->has_value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_IS_NONE_BY_VALUE(Option_int, option_int)
it(option_int_is_none, "must return true when Option has no value") {
  Option_int opt = (Option_int) { .has_value = false };
  assert_true(option_int_is_none(&opt), "Expected None");
}
#endif

#define DEFINE_OPTION_IS_SOME_BY_VALUE(SELFNAME, SELFPREFIX) \
/** Return true when Option contains value, or false otherwise. */ \
NN WUR MU SI bool SELFPREFIX##_is_some(const SELFNAME * self) { \
  return self->has_value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_IS_SOME_BY_VALUE(Option_int, option_int)
it(option_int_is_some, "must return true when Option has value") {
  Option_int opt = (Option_int) { .has_value = true };
  assert_true(option_int_is_some(&opt), "Expected Some");
  assert_true(!option_int_is_none(&opt), "Expected Some");
}
#endif

#define DEFINE_OPTION_SOME_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
/** Return Some with given value. */ \
WUR MU SI SELFNAME SELFPREFIX##_some(CTYPE value) { \
  return (SELFNAME) { .has_value = true, .value = value }; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_SOME_BY_VALUE(Option_int, option_int, int)
it(option_int_some, "must return Some") {
  Option_int opt = option_int_some(42);
  assert_true(option_int_is_some(&opt), "Expected Some");
  assert_equal_int(42, opt.value, "option_int_some() must initialize Option with given value");
}
#endif

#define DEFINE_OPTION_NONE_BY_VALUE(SELFNAME, SELFPREFIX) \
/** Return None. */ \
WUR MU SI SELFNAME SELFPREFIX##_none() { \
  return (SELFNAME) { .has_value = false }; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_NONE_BY_VALUE(Option_int, option_int)
it(option_int_none, "must return None") {
  Option_int opt = option_int_none();
  assert_true(option_int_is_none(&opt), "Expected None");
}
#endif

#define DEFINE_OPTION_DEFAULT(SELFNAME, SELFPREFIX) \
/** Return default value for Option: None. */ \
WUR MU SI SELFNAME SELFPREFIX##_default() { \
  return SELFPREFIX##_none(); \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_DEFAULT(Option_int, option_int)
it(option_int_default, "must return None") {
  Option_int opt = option_int_default();
  assert_true(option_int_is_none(&opt), "Expected None");
}
#endif

#define DEFINE_OPTION_EXPECT(SELFNAME, SELFPREFIX, CTYPE) \
/** Return value or panic with given message, if None. */ \
WUR MU SI CTYPE SELFPREFIX##_expect(const SELFNAME self, const char * message) { \
  if(SELFPREFIX##_is_none(&self)) { \
    option_panic(OPTION_ERROR_VALUE_EXPECTED, message); \
  } \
  return self.value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_EXPECT(Option_int, option_int, int)
it(option_int_expect, "must return value or panic with message when None") {
  Option_int opt = option_int_some(42);
  assert_equal_int(42, option_int_expect(opt, "a message"), "must return value");
  opt = option_int_none();
  assert_abort((void)(42 == option_int_expect(opt, "a message")), "must panic");
}
#endif

#define DEFINE_OPTION_UNWRAP(SELFNAME, SELFPREFIX, CTYPE) \
/** Return value or panic, if None. */ \
MU SI CTYPE SELFPREFIX##_unwrap(const SELFNAME self) { \
  return SELFPREFIX##_expect(self, "unwrap() is called on None"); \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_UNWRAP(Option_int, option_int, int)
it(option_int_unwrap, "must return value or panic with default message when None") {
  Option_int opt = option_int_some(42);
  assert_equal_int(42, option_int_unwrap(opt), "must return value");
  opt = option_int_none();
  assert_abort((void)(42 == option_int_unwrap(opt)), "must panic");
}
#endif

#define DEFINE_OPTION_UNWRAP_OR(SELFNAME, SELFPREFIX, CTYPE) \
/** Return value or default value, if None. */ \
WUR MU SI CTYPE SELFPREFIX##_unwrap_or(const SELFNAME self, CTYPE default_value) { \
  if(SELFPREFIX##_is_none(&self)) \
      return default_value; \
  return self.value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_UNWRAP_OR(Option_int, option_int, int)
it(option_int_unwrap_or, "must return value or return default value when None") {
  Option_int opt = option_int_some(42);
  assert_equal_int(42, option_int_unwrap_or(opt, 53), "must return value");
  opt = option_int_none();
  assert_equal_int(53, option_int_unwrap_or(opt, 53), "must return default value");
}
#endif

#define DEFINE_OPTION_AND(SELFNAME, SELFPREFIX) \
/** Return None, when self is None, or other Option. */ \
WUR MU SI SELFNAME SELFPREFIX##_and(const SELFNAME self, const SELFNAME other) { \
  if(SELFPREFIX##_is_none(&self)) \
      return self; \
  return other; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_AND(Option_int, option_int)
it(option_int_and, "must return None or other option") {
  const Option_int some42 = option_int_some(42);
  const Option_int some53 = option_int_some(53);
  const Option_int none = option_int_none();

  assert_equal_int(53, option_int_unwrap(option_int_and(some42, some53)), "must return value of other option");
  assert_equal_int(64, option_int_unwrap_or(option_int_and(some42, none), 64), "must return value of other option");
  assert_equal_int(64, option_int_unwrap_or(option_int_and(none, some53), 64), "must return none");
  assert_equal_int(64, option_int_unwrap_or(option_int_and(none, none), 64), "must return none");
}
#endif

#define DEFINE_OPTION_OR(SELFNAME, SELFPREFIX) \
/** Return Some, when self is Some, or other Option. */ \
WUR MU SI SELFNAME SELFPREFIX##_or(const SELFNAME self, const SELFNAME other) { \
  if(SELFPREFIX##_is_some(&self)) \
      return self; \
  return other; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_OR(Option_int, option_int)
it(option_int_or, "must return Some or other option") {
  const Option_int some42 = option_int_some(42);
  const Option_int some53 = option_int_some(53);
  const Option_int none = option_int_none();

  assert_equal_int(42, option_int_unwrap(option_int_or(some42, some53)), "must return self");
  assert_equal_int(42, option_int_unwrap(option_int_or(some42, none)), "must return self");
  assert_equal_int(53, option_int_unwrap(option_int_or(none, some53)), "must return other option");
  assert_equal_int(64, option_int_unwrap_or(option_int_or(none, none), 64), "must return none");
}
#endif

#define DEFINE_OPTION_GET_OR_INSERT_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
/** Initialize this option with default value, if necessary, then return it. */ \
NN WUR MU SI CTYPE SELFPREFIX##_get_or_insert(SELFNAME * self, CTYPE default_value) { \
  if(SELFPREFIX##_is_none(self)) { \
      self->value = default_value; \
      self->has_value = true; \
  } \
  return self->value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_GET_OR_INSERT_BY_VALUE(Option_int, option_int, int)
it(option_int_get_or_insert, "must initialize option with default value, if necessary, and return Some") {
  Option_int none = option_int_none();

  assert_equal_int(42, option_int_get_or_insert(&none, 42), "must return default value");
  assert_equal_int(42, option_int_unwrap(none), "must return value");

  Option_int some = option_int_some(53);
  assert_equal_int(53, option_int_get_or_insert(&some, 42), "must return value");
  assert_equal_int(53, option_int_unwrap(some), "must return value");
}
#endif

#define DEFINE_OPTION_TAKE_BY_VALUE(SELFNAME, SELFPREFIX) \
/** Return copy of self and set self to None. */ \
NN WUR MU SI SELFNAME SELFPREFIX##_take(SELFNAME * self) { \
  SELFNAME copy = { .has_value = self->has_value, .value = self->value }; \
  self->has_value = false; \
  return copy; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_TAKE_BY_VALUE(Option_int, option_int)
it(option_int_get_take, "must return copy of self and set itself to None") {
  Option_int some = option_int_some(42);

  assert_equal_int(42, option_int_unwrap(option_int_take(&some)), "must return copy of self");
  assert_equal_int(53, option_int_unwrap_or(some, 53), "must return none after value is taken");

  Option_int none = option_int_none();
  assert_equal_int(53, option_int_unwrap_or(option_int_take(&none), 53), "must return None");
}
#endif

#define DEFINE_OPTION_CLONE_BY_VALUE(SELFNAME, SELFPREFIX) \
/** Return shallow copy of Option. */ \
NN WUR MU SI SELFNAME SELFPREFIX##_clone(const SELFNAME * self) { \
  return (SELFNAME){ .has_value = self->has_value, .value = self->value }; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_CLONE_BY_VALUE(Option_int, option_int)
it(option_int_get_clone, "must return copy of self") {
  Option_int some = option_int_some(42);

  assert_equal_int(42, option_int_unwrap(option_int_clone(&some)), "must return copy of self");

  Option_int none = option_int_none();
  assert_equal_int(53, option_int_unwrap_or(option_int_clone(&none), 53), "must return copy of self");
}
#endif

#define DEFINE_OPTION_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_STRUCT_BY_VALUE(SELFNAME, CTYPE) \
  DEFINE_OPTION_IS_NONE_BY_VALUE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_IS_SOME_BY_VALUE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_SOME_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_NONE_BY_VALUE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_DEFAULT(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_EXPECT(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_UNWRAP(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_UNWRAP_OR(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_AND(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_OR(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_GET_OR_INSERT_BY_VALUE(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_TAKE_BY_VALUE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_CLONE_BY_VALUE(SELFNAME, SELFPREFIX) \

//
// Option by reference
//

#define DEFINE_OPTION_STRUCT_BY_REFERENCE(SELFNAME, CTYPE) \
/** Option for pointers. None is equal to NULL. */ \
typedef struct SELFNAME##_s { \
  CTYPE value; \
} SELFNAME;
#ifdef _CRUST_TESTS
DEFINE_OPTION_STRUCT_BY_REFERENCE(Option_charp, char *)
#endif


#define DEFINE_OPTION_IS_NONE_BY_REFERENCE(SELFNAME, SELFPREFIX) \
/** Return true when None, or false otherwise. */ \
NN WUR MU SI bool SELFPREFIX##_is_none(const SELFNAME * self) { \
  return !self->value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_IS_NONE_BY_REFERENCE(Option_charp, option_charp)
it(option_charp_is_none, "must return true when Option has no value") {
  Option_charp opt = (Option_charp) { .value = NULL };
  assert_true(option_charp_is_none(&opt), "Expected None");
}
#endif

#define DEFINE_OPTION_IS_SOME_BY_REFERENCE(SELFNAME, SELFPREFIX) \
/** Return true when Option contains value, or false otherwise. */ \
NN WUR MU SI bool SELFPREFIX##_is_some(const SELFNAME * self) { \
  return !!self->value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_IS_SOME_BY_REFERENCE(Option_charp, option_charp)
it(option_charp_is_some, "must return true when Option has value") {
  Option_charp opt = (Option_charp) { .value = "" };
  assert_true(option_charp_is_some(&opt), "Expected Some");
  assert_true(!option_charp_is_none(&opt), "Expected Some");
}
#endif

#define DEFINE_OPTION_SOME_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE) \
/** Return Some with given value. */ \
WUR MU SI SELFNAME SELFPREFIX##_some(CTYPE value) { \
  if(!value) { \
      option_panic(OPTION_ERROR_VALUE_EXPECTED, "Option some() called with NULL pointer."); \
  } \
  return (SELFNAME) { .value = value }; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_SOME_BY_REFERENCE(Option_charp, option_charp, char *)
it(option_charp_some, "must return Some") {
  Option_charp opt = option_charp_some("foo");
  assert_true(option_charp_is_some(&opt), "Expected Some");
  assert_equal_charp("foo", opt.value, "option_charp_some() must initialize Option with given value");
}
#endif

#define DEFINE_OPTION_NONE_BY_REFERENCE(SELFNAME, SELFPREFIX) \
/** Return None. */ \
WUR MU SI SELFNAME SELFPREFIX##_none() { \
  return (SELFNAME) { .value = NULL }; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_NONE_BY_REFERENCE(Option_charp, option_charp)
it(option_charp_none, "must return None") {
  Option_charp opt = option_charp_none();
  assert_true(option_charp_is_none(&opt), "Expected None");
}
#endif

#ifdef _CRUST_TESTS
DEFINE_OPTION_DEFAULT(Option_charp, option_charp)
it(option_charp_default, "must return None") {
  Option_charp opt = option_charp_default();
  assert_true(option_charp_is_none(&opt), "Expected None");
}

DEFINE_OPTION_EXPECT(Option_charp, option_charp, char *)
it(option_charp_expect, "must return value or panic with message when None") {
  Option_charp opt = option_charp_some("foo");
  assert_equal_charp("foo", option_charp_expect(opt, "a message"), "must return value");
  opt = option_charp_none();
  assert_abort((void)(NULL == option_charp_expect(opt, "a message")), "must panic");
}

DEFINE_OPTION_UNWRAP(Option_charp, option_charp, char *)
it(option_charp_unwrap, "must return value or panic with default message when None") {
  Option_charp opt = option_charp_some("foo");
  assert_equal_charp("foo", option_charp_unwrap(opt), "must return value");
  opt = option_charp_none();
  assert_abort((void)(NULL == option_charp_unwrap(opt)), "must panic");
}

DEFINE_OPTION_UNWRAP_OR(Option_charp, option_charp, char *)
it(option_charp_unwrap_or, "must return value or return default value when None") {
  Option_charp opt = option_charp_some("foo");
  assert_equal_charp("foo", option_charp_unwrap_or(opt, "bar"), "must return value");
  opt = option_charp_none();
  assert_equal_charp("bar", option_charp_unwrap_or(opt, "bar"), "must return default value");
}

DEFINE_OPTION_AND(Option_charp, option_charp)
it(option_charp_and, "must return None or other option") {
  const Option_charp somefoo = option_charp_some("foo");
  const Option_charp somebar = option_charp_some("bar");
  const Option_charp none = option_charp_none();

  assert_equal_charp("bar", option_charp_unwrap(option_charp_and(somefoo, somebar)), "must return value of other option");
  assert_equal_charp("baz", option_charp_unwrap_or(option_charp_and(somefoo, none), "baz"), "must return value of other option");
  assert_equal_charp("baz", option_charp_unwrap_or(option_charp_and(none, somebar), "baz"), "must return none");
  assert_equal_charp("baz", option_charp_unwrap_or(option_charp_and(none, none), "baz"), "must return none");
}

DEFINE_OPTION_OR(Option_charp, option_charp)
it(option_charp_or, "must return Some or other option") {
  const Option_charp somefoo = option_charp_some("foo");
  const Option_charp somebar = option_charp_some("bar");
  const Option_charp none = option_charp_none();

  assert_equal_charp("foo", option_charp_unwrap(option_charp_or(somefoo, somebar)), "must return self");
  assert_equal_charp("foo", option_charp_unwrap(option_charp_or(somefoo, none)), "must return self");
  assert_equal_charp("bar", option_charp_unwrap(option_charp_or(none, somebar)), "must return other option");
  assert_equal_charp("baz", option_charp_unwrap_or(option_charp_or(none, none), "baz"), "must return none");
}
#endif

#define DEFINE_OPTION_GET_OR_INSERT_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE) \
/** Initialize this option with default value, if necessary, then return it. */ \
__attribute__((nonnull(1))) \
WUR MU SI CTYPE SELFPREFIX##_get_or_insert(SELFNAME * self, CTYPE default_value) { \
  if(SELFPREFIX##_is_none(self)) { \
      if(!default_value) { \
	  option_panic(OPTION_ERROR_VALUE_EXPECTED, "get_or_insert() called with NULL instead of default value"); \
      } \
      self->value = default_value; \
  } \
  return self->value; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_GET_OR_INSERT_BY_REFERENCE(Option_charp, option_charp, char *)
it(option_charp_get_or_insert, "must initialize option with default value, if necessary, and return Some") {
  Option_charp none = option_charp_none();

  assert_equal_charp("foo", option_charp_get_or_insert(&none, "foo"), "must return default value");
  assert_equal_charp("foo", option_charp_unwrap(none), "must return value");

  Option_charp some = option_charp_some("baz");
  assert_equal_charp("baz", option_charp_get_or_insert(&some, "foo"), "must return value");
  assert_equal_charp("baz", option_charp_unwrap(some), "must return value");
}
#endif

#define DEFINE_OPTION_TAKE_BY_REFERENCE(SELFNAME, SELFPREFIX) \
/** Return copy of self and set self to None. */ \
NN WUR MU SI SELFNAME SELFPREFIX##_take(SELFNAME * self) { \
  SELFNAME copy = { .value = self->value }; \
  self->value = NULL; \
  return copy; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_TAKE_BY_REFERENCE(Option_charp, option_charp)
it(option_charp_get_take, "must return copy of self and set itself to None") {
  Option_charp some = option_charp_some("foo");

  assert_equal_charp("foo", option_charp_unwrap(option_charp_take(&some)), "must return copy of self");
  assert_equal_charp("bar", option_charp_unwrap_or(some, "bar"), "must return none after value is taken");

  Option_charp none = option_charp_none();
  assert_equal_charp("bar", option_charp_unwrap_or(option_charp_take(&none), "bar"), "must return None");
}
#endif

#define DEFINE_OPTION_CLONE_BY_REFERENCE(SELFNAME, SELFPREFIX, TYPEPREFIX) \
/** Return deep copy of Option. */ \
NN WUR MU SI SELFNAME SELFPREFIX##_clone(const SELFNAME * self) { \
  return (SELFNAME){ .value = self->value? TYPEPREFIX##_clone(self->value) : NULL }; \
}
#ifdef _CRUST_TESTS
DEFINE_OPTION_CLONE_BY_REFERENCE(Option_charp, option_charp, charp)
it(option_charp_get_clone, "must return copy of self") {
  Option_charp some = option_charp_some("foo");

  defer(charp_destroy) char * ptr = option_charp_unwrap(option_charp_clone(&some));
  assert_equal_charp("foo", ptr, "must return clone of self");
  assert_true(some.value != ptr, "must return clone of self, not copy");

  Option_charp none = option_charp_none();
  assert_equal_charp("bar", option_charp_unwrap_or(option_charp_clone(&none), "bar"), "must return copy of self");
}
#endif


#define DEFINE_OPTION_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE, TYPEPREFIX) \
  DEFINE_OPTION_STRUCT_BY_REFERENCE(SELFNAME, CTYPE) \
  DEFINE_OPTION_IS_NONE_BY_REFERENCE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_IS_SOME_BY_REFERENCE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_SOME_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_NONE_BY_REFERENCE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_DEFAULT(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_EXPECT(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_UNWRAP(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_UNWRAP_OR(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_AND(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_OR(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_GET_OR_INSERT_BY_REFERENCE(SELFNAME, SELFPREFIX, CTYPE) \
  DEFINE_OPTION_TAKE_BY_REFERENCE(SELFNAME, SELFPREFIX) \
  DEFINE_OPTION_CLONE_BY_REFERENCE(SELFNAME, SELFPREFIX, TYPEPREFIX) \

#endif /* OPTION_H */
