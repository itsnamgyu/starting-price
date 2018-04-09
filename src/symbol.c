#include "symbol.h"
#include "generic_dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Recall these definitions
#define BASE_SIZE 10
#define SYMBOL_LENGTH 6

typedef struct _Symbol {
	unsigned int address;
} Symbol;

typedef Dict SymbolDict;
*/

static const int SYMBOL_DICT_SIZE = 40;

#ifdef TEST
#undef TEST
#include "generic_dict.c"
#define TEST

int main(void) {
	{
		SymbolDict *dict = new_symbol_dict();

		add_to_symbol_dict(dict, "Hello", 1023);
		add_to_symbol_dict(dict, "Yo", 2039);

		unsigned int address;

		assert(find_symbol_address(dict, "Hello", &address));
		assert(address == 1023);
		assert(find_symbol_address(dict, "Yo", &address));
		assert(address == 2039);

		free_symbol_dict(dict);
	}

	printf("----------------------------------------\n");
	printf("Automatic tests successful!\n");

#ifdef EXPLICIT
	{
		SymbolDict *dict = new_symbol_dict();

		add_to_symbol_dict(dict, "Hello", 512);
		add_to_symbol_dict(dict, "Yo", 1024);
		add_to_symbol_dict(dict, "Yolo", 2048);
		add_to_symbol_dict(dict, "Abs", 1536);
		add_to_symbol_dict(dict, "Hoho", 256);

		fprint_symbols(stdout, dict);

		free_symbol_dict(dict);
	}
#endif

	return 0;
}
#endif


SymbolDict *new_symbol_dict() {
	return new_dict(SYMBOL_DICT_SIZE);
}

void add_to_symbol_dict(SymbolDict *dict, char *string, unsigned int address) {
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol->address = address;
	add_to_dict(dict, string, symbol);
}

int find_symbol_address(SymbolDict *dict, char *string, unsigned int *address) {
	Symbol *symbol;
	if (find_from_dict(dict, string, (void**) &symbol)) {
		*address = symbol->address;
		return 1;
	} else
		return 0;
}

void free_symbol_dict(SymbolDict *dict) {
	free_dict(dict, free);
}

// Note that typedef and structs are static by default
typedef struct _SymbolPair {
	char *string;
	unsigned int address;
} SymbolPair;

static int compare_symbol_pairs(const void *a, const void *b) {
	return - strcmp(((SymbolPair*)a)->string, ((SymbolPair*)b)->string);
}

// Note that this function is reliant on the implementation of Dict
void fprint_symbols(FILE *out, SymbolDict *dict) {
	int count = 0;
	for (int i = 0; i < dict->size; ++i) {
		DictNode list = dict->nodes[i];
		for (BucketNode *node = list.head->link; node; node = node->link) {
			count ++;
		}
	}

	SymbolPair *pairs = malloc(sizeof(SymbolPair) * count);
	int index = 0;
	for (int i = 0; i < dict->size; ++i) {
		DictNode list = dict->nodes[i];
		for (BucketNode *node = list.head->link; node; node = node->link) {
			pairs[index].string = node->key;
			pairs[index++].address = ((Symbol*)node->value)->address;
		}
	}

	qsort(pairs, count, sizeof(SymbolPair), compare_symbol_pairs);
	
	for (int i = 0; i < count; ++i) {
		SymbolPair pair = pairs[i];
		fprintf(out, "\t%s\t%04X\n", pair.string, pair.address);
	}
}
