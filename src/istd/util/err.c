#include "istd/util/err.h"
#include "istd/util/tty.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define ESC_PANIC   ESC_RED ESC_BOLD
#define ESC_NOTE    ESC_BLUE

__attribute__((noreturn))
__attribute__((format(printf, 2, 3)))
void _istd_internal_panic(const char* location, const char* fmt, ...) {
  
  va_list args;
  va_start(args, fmt);

  fprintf(stderr, "\n");
  fprintf(stderr, ESC_PANIC "Unrecoverable panic:" ESC_RESET "\n");
  fprintf(stderr, "\n");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  fprintf(stderr, ESC_NOTE "Note: " ESC_RESET "This panic occured at %s\n", location);
  exit(EXIT_FAILURE);
}


void* _istd_calloc_checked(size_t num, size_t item_size, const char* errmsg) {

  void* mem = calloc(num, item_size);

  if (mem == NULL) {
    if (errmsg)
      panic$("%s\n", errmsg);
    else
      panic$("Failed to calloc(len: %zu, item_size: %zu)\n", num, item_size);
  }

  return mem;
}
