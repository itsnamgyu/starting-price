#include "help.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
// Test functions in the help module
// Refer to the README on testing instructions
int main(void) {
	print_help(stdout);

	return 0;
}
#endif

void fprint_help(FILE *out) {
	const int HELP_COUNT = 10;
	const char *HELP_STRINGS[] = {
		"h[elp]", "d[ir]",
		"q[uit]", "hi[story]",
		"du[mp] [start, end]", "e[dit] address, value",
		"f[ill] start, end, value", "reset",
		"opcode mnemonic", "opcodelist"
	};

	for (int i = 0; i < HELP_COUNT; ++i)
		fprintf(out, "%s\n", HELP_STRINGS[i]);
}
