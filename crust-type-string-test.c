#include "crust-type-string.h"
#include "crust-unittest.h"

it(string_default, "must return empty String builder") {
  defer(string_destroy) String s = string_default();
  assert_equal_int(0, string_len(&s), "Unexpected length of default String builder");
  assert_equal_int(1, string_capacity(&s), "Unexpected capacity of default String builder");
}

it(string_new_push_get_destroy, "") {
  defer(string_destroy) String str = string_new();

  for(int i=0; i<26; i++) {
    string_push(&str, 'a'+i);
  }

  // Place '\0' at the end of the string. Required after push() to export into char *.
  string_end_with_zero(&str);

  assert_equal_int(26, string_len(&str), "Unexpected size of the String after string_push()");

  for(size_t i=0; i<string_len(&str); i++) {
    assert_equal_int('a'+i, string_get(&str, i), "Unexpected value of item after vec_int_from_raw_parts()");
  }

  assert_equal_charp("abcdefghijklmnopqrstuvwxyz", string_as_ptr(&str), "Unexpected value of string after string_push() and string_putz()");
}

it(string_put_char, "must append char to String builder with checking of bounds and with '\\0' at end") {
  defer(string_destroy) String str = string_new();

  for(int i=0; i<3; i++) {
    string_put_char(&str, 'a'+i);
  }

  assert_equal_charp("abc", string_as_ptr(&str), "Unexpected value of string after string_put_char()");
}

it(string_put_string, "must append C string to String builder with checking of bounds and with '\\0' at end") {
  defer(string_destroy) String str = string_new();

  for(int i=0; i<3; i++) {
    string_put_charp(&str, "123! ");
  }

  assert_equal_charp("123! 123! 123! ", string_as_ptr(&str), "Unexpected value of string after string_put_charp()");
}

it(string_printf, "must append formatted string to String builder with checking of bounds and with '\\0' at end") {
  defer(string_destroy) String str = string_new();

  for(int i=0; i<3; i++) {
    string_printf(&str, "%s%d ", "item#", i);
  }

  assert_equal_charp("item#0 item#1 item#2 ", string_as_ptr(&str), "Unexpected value of string after string_printf()");
}
