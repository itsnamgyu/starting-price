#pragma once
#include <stdio.h>

#include "generic_dict.h"

typedef struct _Opcode {
	unsigned char raw;
} Opcode;

#define HashTable Dict

HashTable *new_hash_table();
/*	Description
 *	Initiate memory for a valid HashTable.
 */

void free_hash_table(HashTable *table);
/*	Description
 *  Free memory and destory HashTable.
 */

void add_to_hash_table(HashTable *table, char *key, unsigned char opcode);
/*	Description
 *  Add a key-value entry to HashTable.
 *
 *  Arguments
 *  char *key: key as a string.
 *  Value value: the value to store. To change the fields of the value later
 *  on, change the definition of struct Value
 */

int find_from_hash_table(HashTable *table, char *key, unsigned char *opcode);
/*	Description
 *  Find the value associated with key from table
 *
 *  Arguments
 *  char *key: key as string
 *  Value *value: return found value to reference
 *
 *  Return
 *  A boolean value for whether the key exists
 */

void fprint_hash_table(FILE *out, HashTable *table);
/*	Description
 *  Prints the hash-table to a human-readable form to an output stream
 */

#if defined(TEST) && !defined(HASHTABLE_C)
#undef TEST
#include "hashtable.c"
#define TEST 0
#endif
