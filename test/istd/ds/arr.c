/**
 * Array tests
 */

#include "istd/util/test.h"
#include <stddef.h>
#include <stdint.h>
#include "istd/ds/arr.h"

itest_section$("default, istd", "ISTD Arrays") {

  itest_case$("Empty arrays") {
    
    ia_arr$(int) empty = ia_new_empty_array$(int);

    itest_check_ptr_notnull$(empty, "Empty array should be allocated");
    itest_die_if_something_failed$();
    itest_check_uint_equal$(ia_length(empty), 0, "Empty array should have zero length");
    itest_check_uint_equal$((uintptr_t) empty % alignof(max_align_t), 0, "Array should be aligned properly");
  }

  itest_case$("Array of given starting size") {
     
    ia_arr$(int) not_empty = ia_new_array_of$(10, int);

    itest_check_ptr_notnull$(not_empty, "Array should be allocated");
    itest_die_if_something_failed$();
    itest_check_uint_equal$(ia_length(not_empty), 10, "Array should have requested length");
    itest_check_uint_ge$(ia_avail(not_empty), 10, "There should be some space availiable");
    itest_check_uint_equal$((uintptr_t) not_empty % alignof(max_align_t), 0, "Array should be aligned properly");
    for (size_t i = 0; i < 10; ++i)
      itest_check_int_equal$(not_empty[i], 0, "Array should be zeroed: %zu-th item", i);
  }

  itest_case$("Arrays with preallocated space") {

    ia_arr$(int) with_prealloc = ia_new_array_for$(10, int);
  
    itest_check_ptr_notnull$(with_prealloc, "Array should be allocated");
    itest_die_if_something_failed$();
    itest_check_uint_equal$(ia_length(with_prealloc), 0, "Array should have requested length");
    itest_check_uint_ge$(ia_avail(with_prealloc), 10, "There should be some space availiable");
    itest_check_uint_equal$((uintptr_t) with_prealloc % alignof(max_align_t), 0, "Array should be aligned properly");

  }

  itest_case$("NULL handling") {

    itest_check_uint_equal$(ia_length(NULL), 0, "Length of NULL should be 0");
    itest_check_uint_equal$(ia_avail(NULL), 0, "Availiable space at NULL should be 0");
  }

  itest_case$("Push() - characters") {

    ia_arr$(char) str = ia_new_empty_array$(char);
    
    itest_check_char_equal$(str[0], '\0', "Array must be null-terminated");
    
    ia_push$(&str, 'a');
    
    itest_check_uint_equal$(ia_length(str), 1, "After push() length must increase to proper value");
    itest_check_uint_ge$(ia_avail(str), 1, "Availiable space must also increase");

    itest_check_char_equal$(str[0], 'a', "Array must contain pushed value");
    itest_check_char_equal$(str[1], '\0', "Array must be null-terminated");
  }

  itest_case$("Push() - structures and multibyte") {
    struct test {
      int foo;
      const char* bar;
    };

    ia_arr$(struct test) struct_array = ia_new_array_of$(1, struct test);

    itest_check_uint_equal$(struct_array[0].foo, 0, "Elements of structure should be zero-initialized");
    itest_check_ptr_equal$(struct_array[0].bar, NULL, "Elements of structure should be zero-initialized");
    
    const char* str = "Hello world!";
    struct test value = { .foo=42, .bar=str };

    ia_push$(&struct_array, value);

    itest_check_uint_equal$(struct_array[1].foo, 42, "Correct value should be pushed");
    itest_check_ptr_equal$(struct_array[1].bar, str, "Correct value should be pushed");
    
  }

  itest_case$("Pop()") {

    ia_arr$(char) str = ia_new_empty_array$(char);
    
    
    ia_push$(&str, 'a');

    itest_check_uint_equal$(ia_length(str), 1, "Array should have an item after it was pushed");
    itest_check_char_equal$(str[1], '\0', "Array must be null-terminated");

    ia_pop$(&str);

    itest_check_uint_equal$(ia_length(str), 0, "Array should decrease its length back after pop");
    itest_check_char_equal$(str[0], '\0', "Array must be null-terminated");
    
  }
}
