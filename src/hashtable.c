#include "hashtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPCODE_TABLE_SIZE 20

/* Recall these definitions
typedef struct _Opcode {
	unsigned char raw;
} Opcode;

#define HashTable Dict;
*/

#ifdef TEST
#undef TEST
#include "generic_dict.c"
// Manual unit test for this module
// Refer to the README for testing instructions
int main(void) {
	for (int i = 0; i < 10; ++i) {

		HashTable *table = new_hash_table();

		add_to_hash_table(table, "Hello!", 12);
		add_to_hash_table(table, "Hoho!", 23);

		for (int i = 0; i < 50; ++i) {
			char ho[3] = { 0 };
			ho[0] = 32 + i;
			ho[1] = 32 + i;
			add_to_hash_table(table, ho, i);
		}

		printf("Print hash table\n");
		fprint_hash_table(stdout, table);

		printf("\n");
		printf("Find non-exisitng\n");
		unsigned char opcode;
		if (find_from_hash_table(table, "Non", &opcode))
			printf("> Found something..? FAIL\n");
		else 
			printf("> Yes! We couldn't find anything\n");

		printf("\n");
		printf("Find Hello!\n");
		if (find_from_hash_table(table, "Hello!", &opcode))
			printf("> Found this: %02X\n", opcode);
		else 
			printf("> Couldn't find anything..? FAIL\n");

		free_hash_table(table);

	}
	return 0;
}
#endif

HashTable *new_hash_table() {
	Dict *dict = new_dict(OPCODE_TABLE_SIZE);
	return dict;
}

void free_hash_table(HashTable *table) {
	free_dict(table, free);
}

void add_to_hash_table(HashTable *table, char *key, unsigned char opcode) {
	Opcode *value = malloc(sizeof(Opcode));
	value->raw = opcode;
	add_to_dict(table, key, (void*) value);
}

int find_from_hash_table(HashTable *table, char *key, unsigned char *opcode) {
	Opcode *value;
	if (find_from_dict(table, key, (void**) &value)) {
		*opcode = value->raw; return 1;
	} else return 0;
}

void fprint_opcode(FILE *out, void *opcode_void) {
	Opcode *opcode = (Opcode*) opcode_void;
	fprintf(out, "%02X", opcode->raw);
}

void fprint_hash_table(FILE *out, HashTable *table) {
	fprint_dict(out, table, fprint_opcode);
}
