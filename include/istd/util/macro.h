/**
 * \file
 * \brief Common macro constructs
 *
 */

#ifndef ISTD_UTIL_MACRO
#define ISTD_UTIL_MACRO

#include <stdlib.h>

// Concatenating ID-s
#define _im_concat_inner$(a, b) a##b
#define im_concat$(a, b) _im_concat_inner$(a, b)


// Stringification of given param
#define _im_str_inner$(a) #a
#define im_str$(a) _im_str_inner$(a)


// Begin/end calls wrapped into `foo {}`
// Used like:
//
//   #define my_widget(ctx, ...) im_with$(my_widget_begin(ctx, __VA_OPT__(,) __VA_ARGS__), my_widget_end(ctx))
//
//   my_widget(ctx, "My title") {
//     ...
//   }
//
#define _im_with_inner$(begin, end, var_name) for(int var_name = ((begin), 0); var_name < 1; ++var_name, (end))
#define im_with$(begin, end) _im_with_inner$(begin, end, im_concat$(_im_with_counter__, __COUNTER__))


/// wrapper around Calloc()
/// NOTE(i-s-d): Does this belong here?
#define calloc$(num, type) ((type*) calloc((num), sizeof(type)))


#endif
