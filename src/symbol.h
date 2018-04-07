#ifndef SYMBOL_H
#define SYMBOL_H
#include "generic_dict.h"

#include <stdio.h>

#define BASE_SIZE 10
#define SYMBOL_LENGTH 6

typedef struct _Symbol {
	unsigned int address;
} Symbol;

typedef Dict SymbolDict;

SymbolDict *new_symbol_dict();

void add_to_symbol_dict(SymbolDict *dict, char *string);

int set_symbol_address(SymbolDict *dict, char *string, unsigned int address);

int find_symbol_address(SymbolDict *dict, char *string, unsigned int *address);

void free_symbol_dict(SymbolDict *dict);

void fprint_symbols(FILE *out, SymbolDict *dict);
#endif
