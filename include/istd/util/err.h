#ifndef ISTD_UTIL_PANIC
#define ISTD_UTIL_PANIC

#include "istd/util/macro.h"

__attribute__((noreturn))
__attribute__((format(printf, 2, 3)))
void _istd_internal_panic(const char* location, const char* fmt, ...);


/// Get current location to print in panic message
#define location$() \
  (__FILE__ ":" im_str$(__LINE__))

/// Panic with given formatted string
#define panic$(...) \
  _istd_internal_panic(location$(), __VA_ARGS__)

#define check$(expr, ...) do {\
    if (!(expr)) panic$(__VA_ARGS__); \
  } while(0)

#endif
