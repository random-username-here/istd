/**
 * \file
 * \brief Some common actions done using escape codes.
 */

#ifndef ISTD_UTIL_TTY
#define ISTD_UTIL_TTY

//---- Colors and other common sequences

#define ESC_GRAY      "\x1b[90m"
#define ESC_RED       "\x1b[91m"
#define ESC_GREEN     "\x1b[92m"
#define ESC_YELLOW    "\x1b[93m"
#define ESC_BLUE      "\x1b[94m"
#define ESC_PURPLE    "\x1b[95m"
#define ESC_AQUA      "\x1b[96m"
#define ESC_WHITE     "\x1b[97m"

#define ESC_BOLD      "\x1b[1m"
//#define ESC_DIM     "\x1b[2m" // do not use, activates thin font in some terminals
#define ESC_ITALIC    "\x1b[3m"
#define ESC_UNDERLINE "\x1b[4m"
#define ESC_DEL       "\x1b[9m"

#define ESC_RESET     "\x1b[0m"

#endif
