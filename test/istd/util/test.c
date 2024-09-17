/**
 * A showcase of the test system
 */

#include "istd/util/test.h"

itest_section$("istd.showcase", "Test library showcase") {
  
  itest_case$("A section which fails") {

    itest_check$(false, "This check should fail");
    itest_check_int_equal$(2+2, 5, "Another failing test");
    itest_check$(true, "A check which succeeds");
  }

  itest_case$("A section which passes") {
    
    itest_check$(true, "Check which passes");
  }
}
