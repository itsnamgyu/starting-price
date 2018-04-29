#pragma once
#include <stdbool.h>

#include "generic_dict.h"

#define REFERENCE_ARRAY_SIZE 10000
typedef struct _Estab {
	int list[REFERENCE_ARRAY_SIZE];
	Dict *dict;
} Estab;
/*
 * External symbol table which is searchable by symbol or reference.
 *
 * list:
 * Saves the address of symbols in an array with the index corresponds to the reference number
 * assigned to the symbol. When for reference numbers that have not been assigned, the address
 * is set to -1. Naturally, the array is initialized with -1.
 *
 * dictionary:
 * Saves the address of symbols in a dictionary of <symbol-address> pairs.
 */

Estab *new_estab();
/*
 * Initialized a new estab and return a reference.
 */

void free_estab(Estab *estab);
/*
 * Deinitialize estab.
 */

bool add_to_estab(Estab *estab, const char *symbol, int reference_number, int address);
/*
 * Add symbol with optional symbol, reference number and the target address.
 *
 * Arguments:
 * reference_number = -1: NULL
 *
 * Note that at least one of symbol or reference number must be specified.
 */

bool assign_reference_number_to_symbol(Estab *estab, const char *symbol, int reference_number);
/*
 * TODO
 *
 */

int find_from_estab_by_symbol(Estab *estab, const char *symbol);
/*
 * Find address from estab using original symbol string
 *
 * Return Values:
 * -1: not found
 * default: the address that was found
 */

int find_from_estab_by_reference_number(Estab *estab, int reference_number);
/*
 * Find address from estab using the reference number.
 *
 * Return Values:
 * -1: not found
 * default: the address that was found
 */


#if defined(TEST) && !defined(ESTAB_C)
#undef TEST
#include "estab.c"
#define TEST 0
#endif
