#include "istd/util/test.h"
#include "istd/util/err.h"
#include "istd/util/tty.h"
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_TAGS 256

//==== Internal structures

typedef struct itest_section_t {
  struct itest_section_t *prev;
  void (*fn)(void);
  char** tags;
  size_t ntags;
  char* tag_string;
  const char* name;
} itest_section_t;

//==== Global variables here

static itest_section_t* last_registered_section;
static jmp_buf          jmpbuf_to_section_die  ;
static size_t           num_failed             ;
static bool             inside_test_case       ;
static bool             case_failed            ;

//==== Implementations

/// Registers given function in list of all functions to be run.
///
void _itest_register_function(
    const char* id,
    const char* name,
    void(*fn)(void)
  ) {

  check$(id, "Test section ID must be not null");
  check$(name, "Test section name must be not NULL");
  check$(fn, "Test section itself must be a function, not a NULL pointer");

  itest_section_t* sec = (itest_section_t*) calloc(1, sizeof(itest_section_t));
  check$(sec, "Should allocate enough memory for storing a test case");

  sec->prev = last_registered_section;
  last_registered_section = sec;
  sec->name = name;
  sec->fn = fn;
  sec->tag_string = strdup(id);
  check$(sec->tag_string, "Should allocate a duplicate of tag list");

  size_t ntags = 0;
  bool letter_found = false;
  const char* tags[MAX_TAGS];
  const char* begin = NULL;
  for (char* c = sec->tag_string; *c != '\0'; ++c) {
    if (isspace(*c)) {}
    else if (*c == ',') {
      if (letter_found) {
        *c = '\0';
        check$(ntags < MAX_TAGS, "There should be no more than %d tags", MAX_TAGS);
        tags[ntags++] = begin;
        letter_found = false;
      }
    } else if (!letter_found) {
      begin = c;
      letter_found = true;
    }
  }
  if (letter_found) {
    check$(ntags < MAX_TAGS, "There should be no more than %d tags", MAX_TAGS);
    tags[ntags++] = begin;
  }

  sec->ntags = ntags;
  sec->tags = (char**) calloc(ntags, sizeof(const char*));
  check$(sec->tags, "Should allocate an array to contain pointers to tags of the section");
  memcpy(sec->tags, tags, sizeof(const char*) * ntags);

}


#define ESC_MOVE_UP_TO_TITLE "\x1b[2A"
#define ESC_MOVE_DOWN_FROM_TITLE "\x1b[2B"


/// Begins given test of the section.
///
void _itest_begin_test(const char* name) {
  
  check$(!inside_test_case, "You shall not nest test cases");
  check$(name, "Test case should have a name");

  num_failed = 0;
  case_failed = false;
  inside_test_case = true;
  fprintf(stderr, "    " ESC_BOLD "%s\n\n" ESC_RESET, name);
}

/// Ends given test
///
void _itest_end_test(void) {
  check$(inside_test_case, "You should end only existing test");
  inside_test_case = false;

  if (!case_failed) {
    fprintf(stderr, ESC_MOVE_UP_TO_TITLE ESC_GREEN "\r ok" ESC_RESET ESC_MOVE_DOWN_FROM_TITLE "\r");
  }
}

/// Mark current test as failed. This does not break the
/// test flow, only `_itest_die()` does that..
///
void _itest_mark_failed(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  check$(fmt, "Format string should be non-null");
  check$(inside_test_case, "You must be inside a test case (itest_case$)");

  ++num_failed;
  case_failed = true;

  if (num_failed == 1) {
    // first failed test, mark the case as failed
    fprintf(stderr, ESC_MOVE_UP_TO_TITLE ESC_RED "\r =>" ESC_RESET ESC_MOVE_DOWN_FROM_TITLE "\r");
  }
  fprintf(stderr, ESC_RED "      Failed: " ESC_RESET);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n\n");
}

/// Stop current test section. No further tests will be
/// run in it.
///
__attribute__((noreturn)) void _itest_die(void)  {
  longjmp(jmpbuf_to_section_die, 0);
}

bool _itest_has_something_failed(void) {
  return num_failed != 0;
}

#if !defined(ITEST_NO_MAIN) && defined(TEST)

#define ESC_HELP_TITLE ESC_UNDERLINE ESC_BOLD ESC_PURPLE
#define ESC_CMD ESC_AQUA
#define ESC_ARG ESC_GREEN

static void print_help() {

  fprintf(stderr, "\nISTD test runner\n\n");
  fprintf(stderr, "Runs tests specified by certain tags, or "
          ESC_ARG "default" ESC_RESET " tag if nothing was specified\n\n");
  fprintf(stderr, ESC_HELP_TITLE "Usage:" ESC_RESET ESC_CMD " tests " ESC_ARG "<TAG>...\n");

  // TODO: add list of tags here
  //       (requires hash set)
}

#undef ESC_HELP_TITLE
#undef ESC_CMD
#undef ESC_ARG

int main (int argc, const char** argv) {

  for (size_t i = 1; i < (size_t) argc; ++i) {
    if (!strcmp(argv[i], "--help")) {
      print_help();
      return 0;
    }
  }

  // A little bit hacky way to run `default` tag when nothing was specified
  const char* fake_argv[] = { "", "default" };
  if (argc == 1) {
    argc = 2;
    argv = fake_argv;
  }

  size_t sections_run = 0, sections_passed = 0;

  for (const itest_section_t* s = last_registered_section; s != NULL; s = s->prev) {

    // TODO: get this thing out of here, use hash sets again.
    bool tag_matched = false;

    for (size_t i = 0; !tag_matched && i < s->ntags; ++i) {
      for (size_t j = 1; !tag_matched && j < (size_t) argc; ++j) {
        if (!strcmp(s->tags[i], argv[j]))
          tag_matched = true;
      }
    }

    if (!tag_matched)
      continue;

    sections_run++;
    fprintf(stderr, "\n" ESC_UNDERLINE ESC_GRAY "## " ESC_PURPLE ESC_BOLD "%s\n\n" ESC_RESET, s->name);

    if (!setjmp(jmpbuf_to_section_die)) {
      s->fn();
      itest_die_if_something_failed$();
      fprintf(stderr, ESC_GREEN "(OK) All tests passed\n" ESC_RESET);
      ++sections_passed;
    } else {
      fprintf(stderr, ESC_RED "(!!) A test had failed\n" ESC_RESET);
    }
    fprintf(stderr, "\n");
  }

  if (!sections_run) {
    fprintf(stderr, "No test sections match tags " ESC_GREEN);
    for (size_t i = 1; i < (size_t) argc; ++i)
      fprintf(stderr, "%s ", argv[i]);
    fprintf(stderr, "\n");
  } else if (sections_passed == sections_run) {
    fprintf(stderr, "" ESC_GREEN "All tests passed\n\n" ESC_RESET);
  } else {
    fprintf(stderr, "" ESC_RED "%zu of %zu test sections have failed \n\n" ESC_RESET, sections_run - sections_passed, sections_run);
  }
}

#endif 
