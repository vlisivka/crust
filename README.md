# crust

C library for backporting Rust code to C

This code is unfinished work, so stay away.

Example:


    it(string_printf, "must append formatted string to String builder with checking of bounds and with '\\0' at end") {
      defer(string_destroy) String str = string_new();
    
      for(int i=0; i<3; i++) {
        string_printf(&str, "%s%d ", "item#", i);
      }
    
      assert_equal_charp("item#0 item#1 item#2 ", string_as_ptr(&str), "Unexpected value of string after string_printf()");
    }
