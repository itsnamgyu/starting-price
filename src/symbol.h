/*
 * symbol.h
 *
 * This module provides the SymbolDict type that you can use to store and
 * retrive symbols and their corresponding instruction addresses.
 *
 * The functions are just wrapper functions for the generic dict type, so
 * they are pretty self explanatory.
 */

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
typedef SymbolDict SymbolTable;

SymbolDict *new_symbol_dict();

void add_to_symbol_dict(SymbolDict *dict, char *string, unsigned int address);

int find_symbol_address(SymbolDict *dict, char *string, unsigned int *address);

void free_symbol_dict(SymbolDict *dict);

void fprint_symbols(FILE *out, SymbolDict *dict);
/*
 * Prints the symbols in *dict* in descending order, based on the string of
 * the symbol.
 */

#define new_symbol_table new_symbol_dict
#define add_to_symbol_table add_to_symbol_dict
#define free_symbol_table free_symbol_dict
#endif
