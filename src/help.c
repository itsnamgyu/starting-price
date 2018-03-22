#include "help.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	print_help(stdout);

	return 0;
}

void print_help(FILE *out) {
	const int HELP_COUNT = 10;
	const int HELP_LENGTH = 1000;
	const char *HELP_STRINGS[] = {
		"h[elp]", "d[ir]",
		"q[uit]", "hi[story]",
		"du[mp] [start, end]", "e[dit] address, value",
		"f[ill] start, end, value", "reset",
		"opcode mnemonic", "opcodelist"
	};

	char *string = malloc(sizeof(char) * HELP_LENGTH);
	char *full_string = string;

	for (int i = 0; i < HELP_COUNT; ++i)
		string += sprintf(string, "%s\n", HELP_STRINGS[i]);

	return full_string;
}
