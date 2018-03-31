#include "help.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
// Manual unit test for this module
// Refer to the README for testing instructions
int main(void) {
	fprint_help(stdout);

	return 0;
}
#endif

void fprint_help(FILE *out) {
	const int HELP_COUNT = 13;
	const char *HELP_STRINGS[] = {
		"h[elp]", "d[ir]",
		"q[uit]", "hi[story]",
		"du[mp] [start, end]", "e[dit] address, value",
		"f[ill] start, end, value", "reset",
		"opcode mnemonic", "opcodelist",
		"assemble filename", "type filename", 
		"symbol"
	};

	for (int i = 0; i < HELP_COUNT; ++i)
		fprintf(out, "%s\n", HELP_STRINGS[i]);
}
