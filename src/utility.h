#pragma once

/*
 * # Utility.h
 *
 * Provides utility functions for simple tasks
 */

char *malloc_strcpy(const char *string);
/*
 * Allocates memory for a copy of *string*, copies the string, and
 * returns the address of the new string.
 *
 * Note. If the *string* is NULL or the length is 0, returns NULL
 */

#define print_line(DESCRIPTION) fprintf(stderr, "File %-16s Line %-3d: %s\n", __FILE__, __LINE__, DESCRIPTION);
/*
 * Debug function that prints the line number along with the specified description when the
 * current line is executed.
 */


#if defined(TEST) && !defined(UTILITY_C)
#undef TEST
#include "utility.c"
#define TEST 0
#endif
