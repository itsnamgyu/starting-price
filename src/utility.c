#include "utility.h"

#include <stdlib.h>
#include <string.h>

char *malloc_strcpy(const char *string) {
	if (string == NULL) return NULL;

	int length = strlen(string);
	if (length == 0) return NULL;

	char *new_string = malloc(sizeof(char) * (length + 1));
	strcpy(new_string, string);

	return new_string;
}
