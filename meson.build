project('istd', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

MY_FLAGS = [
  
  #---- Warnings, generated only by CLANG.
  # 
  # GCC does not complain about them.

  # This is used ONLY for macros, do not panic about it.
  # But this rule is a good one for variable/function names,
  # because some assemblers cannot tolerate symbols
  # with those names (as I was told by GCC docs).
  '-Wno-dollar-in-identifier-extension',
  
  # This should be removed from here, but for now it is ignored.
  # FIXME, I should find which standard is this, because
  # macros with zero variadic arguments (and __VA_OPT__()) are
  # non-existent in standards. Officially they are supported from C++17,
  # but no mentions about C.
  '-Wno-gnu-zero-variadic-macro-arguments'
]

incdir = include_directories('include')

# Setup arrays to collect filenames into
sources = []
tests = []

# Call setup for subdirs
# subdir('include') # No file for now, because it is not needed
subdir('src')
subdir('test')

# Main library

lib_istd = shared_library(
  'istd',
  sources,
  c_args: MY_FLAGS,
  include_directories : incdir
)

dep = declare_dependency(
  include_directories : incdir,
  link_with : lib_istd
)

# Executable for tests

tests = executable(
  'tests',
  tests + sources,
  include_directories : incdir,
  c_args: [ '-DTEST' ] + MY_FLAGS
)

test('tests', tests)

