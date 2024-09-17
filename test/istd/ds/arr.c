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
    
    itest_fail$("Should write some tests");
  }

  itest_case$("NULL handling") {

    itest_check_uint_equal$(ia_length(NULL), 0, "Length of NULL should be 0");
    itest_check_uint_equal$(ia_avail(NULL), 0, "Availiable space at NULL should be 0");
  }

  itest_case$("Push()") {

    itest_fail$("Should write some tests");
  }

  itest_case$("Pop()") {

    itest_fail$("Should write some tests");
  }
}
