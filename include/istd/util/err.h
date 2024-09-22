#ifndef ISTD_UTIL_PANIC
#define ISTD_UTIL_PANIC

#include "istd/util/macro.h"
#include <stdlib.h>

__attribute__((noreturn))
__attribute__((format(printf, 2, 3)))
void _istd_internal_panic(const char* location, const char* fmt, ...);

void* _istd_calloc_checked(size_t num, size_t item_size, const char* errmsg);

/// Get current location to print in panic message
#define location$() \
  (__FILE__ ":" im_str$(__LINE__))


/// Panic with given formatted string
#define panic$(...) \
  _istd_internal_panic(location$(), __VA_ARGS__)


/// Check what given expresion is valid
#define check$(expr, ...) do {\
    if (!(expr)) panic$(__VA_ARGS__); \
  } while(0)


//---- Allocation with error checking

/// \brief Allocate memory and fail if it wasn't allocated
///
/// If `msg` is `NULL`, it will print the default error message
///
#define calloc_checked$(num, type, msg) \
  ((type*) _istd_calloc_checked((num), sizeof(type), (msg)))
  
  
#endif
