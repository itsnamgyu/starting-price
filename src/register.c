#include "register.h"

#include <stdlib.h>

const char *register_to_string(Register reg) {
	static char *strings[8] = {
		"A", "X", "L", "B", "S", "T", "PC", "SW" };
	static Register registers[8] = {
		A, X, L, B, S, T, PC, SW };
	
	for (int i = 0; i < 8; ++i) {
		if (registers[i] == reg) {
			return strings[i];
		}
	}

	return NULL;
}
