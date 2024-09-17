/**
 * \file
 * \brief A unit testing thing, version 2. No more `_Generic` here.
 */

#ifndef ISTD_UTIL_TEST
#define ISTD_UTIL_TEST

#include "istd/util/macro.h"
#include "istd/util/tty.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//==== Internals, used by macros

/// Registers given function in list of all functions to be run.
///
void _itest_register_function(
    const char* id,
    const char* name,
    void(*fn)(void)
  );

/// Begins given test of the section.
///
void _itest_begin_test(const char* name);

/// Ends given test
///
void _itest_end_test(void);

/// Mark current test as failed. This does not break the
/// test flow, only `_itest_die()` does that..
///
void _itest_mark_failed(const char* fmt, ...);

/// Stop current test section. No further tests will be
/// run in it.
///
__attribute__((noreturn)) void _itest_die(void);

bool _itest_has_something_failed(void);


//==== Macros themselves


//---- Test section


// Internals
// Some magic here, but no a lot.
#define _itest_section_internal$(id, name, test_fn, user_fn)                   \
  /* Predefine user function */                                                \
  static void user_fn (void);                                                  \
  /* Function with test case registering */                                    \
  __attribute__((constructor)) static void test_fn (void) {                    \
    _itest_register_function((id), (name), &user_fn);                          \
  }                                                                            \
                                                                               \
  /* Function with user code */                                                \
  static void user_fn (void)


/// \brief A test section.
///
/// Used like this:
///
///   int sum(int a, int b) {
///     return a + b;
///   }
///
///   itest_section$("mylib/math/sum", "Summation tests") {
///
///     itest_check_int_equal (
///       sum(1, 2), 3,
///       "Sum() should do something sensible"
///     );
///
///   }
///
#define itest_section$(id, name)                                               \
    _itest_section_internal$(                                                  \
        id, name,                                                              \
        im_concat$(_itest_section_init__, __COUNTER__),                        \
        im_concat$(_itest_section__, __COUNTER__)                              \
    )


//---- Test case


/// \brief A test case.
///
/// A test case, used to separate asserts into blocks.
/// For example, array comparsion requires a lot of asserts,
/// but printing all of them if everything is OK is not required.
/// Only failing one should be printed.
///
/// So, you group your assertions into cases like this:
///
///   itest_case$("Arrays should be equal") {
///
///     for (size_t i = 0; i < n; ++i)
///       itest_check_int_equal (a[i], b[i], "element %zu should be same in both arrays", i);
///     
///   }
///
/// ... and something like this will be printed:
///
///   FAIL  Arrays should be equal
///    => Assertion failed: element 3 should be same in both arrays
///       a[i] = 0
///       b[i] = 42
///       a[i] should be equal to b[i]
///
#define itest_case$(name)                                                      \
    im_with$(                                                                  \
        _itest_begin_test(name),                                               \
        _itest_end_test()                                                      \
    )


/// \brief Quit current section.
///
/// After some assertion failed, tests will continue running
/// in that section. This makes sense, because when checking
/// equality of some array you would want to assert equality
/// of all the items, and not quit on first wrong item just
/// to discover on next run what second was also wrong.
///
/// But sometimes something goes wrong, and you cannot
/// proceed. For example, when NULL was returned instead of
/// object, you cannot assert its properties. In that case
/// you stop current section.
///
/// This macro does exactly that.
///
#define itest_die$() _itest_die()


/// \brief Quit current section if some assert has failed.
///
/// In comparsion to `itest_die$()` does nothing when all
/// asserts passed. This way is preferred over `itest_die$()`.
///
#define itest_die_if_something_failed$()                                       \
  if (_itest_has_something_failed()) { itest_die$(); }


//==== Checks

//---- Common colors

#define _ITEST_ESC_EXPR ESC_BLUE
#define _ITEST_ESC_A    ESC_RED
#define _ITEST_ESC_B    ESC_GREEN
#define _ITEST_ESC_C    ESC_AQUA

//---- Padded printf

#define _itest_print$(...) fprintf(stderr, "        " __VA_ARGS__);

//---- Basic checks

/// Fail. Just fail with message, not checking anything.
#define itest_fail$(...) do {                                                  \
    _itest_mark_failed(__VA_ARGS__);                                           \
    _itest_print$("\n");                                                       \
  } while(0);

/// Check what some condition is met.
#define itest_check$(condition, ...)                                           \
  if (!(condition)) {                                                          \
    _itest_mark_failed(__VA_ARGS__);                                           \
    _itest_print$("Expected " _ITEST_ESC_EXPR "%s" ESC_RESET " to be "         \
                  _ITEST_ESC_C "true\n" ESC_RESET,                             \
                  #condition);                                                 \
    _itest_print$("\n");                                                       \
  }

//---- Comparsion using given operator, internals

// Printf() does not handle well with argument stringification...
#define _itest_check_op_internal$(a, b, op, a_var, b_var, type, fmt, ...) do { \
  type a_var = (a);                                                            \
  type b_var = (b);                                                            \
  if (!(a_var op b_var)) {                                                     \
    _itest_mark_failed(__VA_ARGS__);                                           \
    _itest_print$("Expected "                                                  \
                  _ITEST_ESC_A);                                               \
    fputs(#a, stderr);                                                         \
    fprintf(stderr, ESC_RESET                                                  \
            " to be " _ITEST_ESC_C im_str$(op) ESC_RESET " "                   \
            _ITEST_ESC_B);                                                     \
    fputs(#b, stderr);                                                         \
    fprintf(stderr, ESC_RESET "\n");                                           \
    _itest_print$("Instead got:\n");                                           \
    _itest_print$("  " _ITEST_ESC_EXPR);                                       \
    fputs(#a, stderr);                                                         \
    fprintf(stderr, ESC_RESET                                                  \
                  ESC_GRAY " = " _ITEST_ESC_A fmt ESC_RESET "\n", a_var);      \
    _itest_print$("  " _ITEST_ESC_EXPR);                                       \
    fputs(#b, stderr);                                                         \
    fprintf(stderr, ESC_RESET                                                  \
                  ESC_GRAY " = " _ITEST_ESC_B fmt ESC_RESET "\n", b_var);      \
    _itest_print$("\n");                                                       \
  }} while(0)


#define _itest_check_op$(a, b, op, type, fmt, ...)                             \
  _itest_check_op_internal$(                                                   \
      a, b, op,                                                                \
      im_concat$(_itest_a_var__, __COUNTER__),                                 \
      im_concat$(_itest_b_var__, __COUNTER__),                                 \
      type, fmt __VA_OPT__(,) __VA_ARGS__                                      \
  )

//---- Comparsion using given operator, specializations
// TODO: make this part nicer

#define itest_check_int_equal$(a, b, ...) _itest_check_op$(a, b, ==, long long         , "%lld", __VA_ARGS__)
#define itest_check_uint_equal$(a, b, ...) _itest_check_op$(a, b, ==, unsigned long long, "%llu", __VA_ARGS__)
#define itest_check_hex_equal$(a, b, ...) _itest_check_op$(a, b, ==, unsigned long long, "0x%llx", __VA_ARGS__)


#define itest_check_int_gt$(a, b, ...) _itest_check_op$(a, b, > , long long         , "%lld", __VA_ARGS__)
#define itest_check_uint_gt$(a, b, ...) _itest_check_op$(a, b, > , unsigned long long, "%llu", __VA_ARGS__)
#define itest_check_hex_gt$(a, b, ...) _itest_check_op$(a, b, > , unsigned long long, "0x%llx", __VA_ARGS__)

#define itest_check_int_lt$(a, b, ...) _itest_check_op$(a, b, < , long long         , "%lld", __VA_ARGS__)
#define itest_check_uint_lt$(a, b, ...) _itest_check_op$(a, b, < , unsigned long long, "%llu", __VA_ARGS__)
#define itest_check_hex_lt$(a, b, ...) _itest_check_op$(a, b, < , unsigned long long, "0x%llx", __VA_ARGS__)

#define itest_check_int_ge$(a, b, ...) _itest_check_op$(a, b, >=, long long         , "%lld", __VA_ARGS__)
#define itest_check_uint_ge$(a, b, ...) _itest_check_op$(a, b, >=, unsigned long long, "%llu", __VA_ARGS__)
#define itest_check_hex_ge$(a, b, ...) _itest_check_op$(a, b, >=, unsigned long long, "0x%llx", __VA_ARGS__)

#define itest_check_int_le$(a, b, ...) _itest_check_op$(a, b, <=, long long         , "%lld", __VA_ARGS__)
#define itest_check_uint_le$(a, b, ...) _itest_check_op$(a, b, <=, unsigned long long, "%llu", __VA_ARGS__)
#define itest_check_hex_le$(a, b, ...) _itest_check_op$(a, b, <=, unsigned long long, "0x%llx", __VA_ARGS__)

#define itest_check_char_equal$(a, b, ...) _itest_check_op$(a, b, ==, char, "%1$d (`%1$d`)", __VA_ARGS__)
#define itest_check_ptr_equal$(a, b, ...)  _itest_check_op$(a, b, ==, void*, "%p", __VA_ARGS__)

#define itest_check_ptr_null$(a, ...)  _itest_check_op$(a, NULL, ==, void*, "%p", __VA_ARGS__)
#define itest_check_ptr_notnull$(a, ...)  _itest_check_op$(a, NULL, != , void*, "%p", __VA_ARGS__)

#endif
