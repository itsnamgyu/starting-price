#pragma once
/*
 * file.h
 *
 * A module for printing the current directory and printing the
 * contents of files.
 */
#include <stdio.h>

void fprint_dir(FILE *out);
/*	Description
 *	Print the current directory to output stream
 */

int fprint_file(FILE *out, char *filename);
/*	Description
 *	Print the contents of 'filename' to output stream
 */
