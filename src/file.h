#include <stdio.h>

#ifndef FILE_H
#define FILE_H
void fprint_dir(FILE *out);
/*	Description
 *	Print the current directory to output stream
 */

int fprint_file(FILE *out, char *filename);
/*	Description
 *	Print the contents of 'filename' to output stream
 */
#endif
