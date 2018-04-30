#define ESTAB_C

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "estab.h"

static void add_symbol_to_esdict(Dict *dict, const char *symbol, int address);
// Add symbol and address to dictionary in estab, abiding to the generic_dict interface.

#define ESTAB_BUCKET_SIZE 20
Estab *new_estab() {
	Estab *estab = malloc(sizeof(Estab));
	
	for (int i = 0; i < REFERENCE_ARRAY_SIZE; ++i)
		estab->list[i] = -1;
	estab->dict = new_dict(ESTAB_BUCKET_SIZE);

	return estab;
}

void free_estab(Estab *estab) {
	free_dict(estab->dict, free);
	free(estab);
}

bool add_to_estab(Estab *estab, const char *symbol, int reference_number, int address) {
	bool add_symbol = (symbol != NULL);
	bool add_reference = (reference_number != -1);

	assert(add_symbol || add_reference && "must specify symbol or reference");
	// should have been checked during OBJ read
	assert(address >= 0 && "address must be non-negative");
	// should have been checked during OBJ read (since it's hex?)
	
	if (add_symbol && dict_contains(estab->dict, symbol)) return false;
	if (add_reference && estab->list[reference_number] != -1) return false;

	if (add_symbol) add_symbol_to_esdict(estab->dict, symbol, address);
	if (add_reference) estab->list[reference_number] = address;

	return true;
}

bool assign_reference_number_to_symbol(Estab *estab, const char *symbol, int reference_number) {
	assert(0 <= reference_number && reference_number < REFERENCE_ARRAY_SIZE);

	int n = find_from_estab_by_symbol(estab, symbol);
	if (n == -1) return false;
	else estab->list[reference_number] = n;

	return true;
}

int find_from_estab_by_symbol(Estab *estab, const char *symbol) {
	int *value;

	if (!find_from_dict(estab->dict, symbol, (void**) &value)) return -1;
	return *value;
}

int find_from_estab_by_reference_number(Estab *estab, int reference_number) {
	assert(reference_number >= 0 && "reference number must be non-negative");
	// should have been checked during OBJ read
	assert(reference_number < REFERENCE_ARRAY_SIZE && "reference number can't be too big");
	// should have been checked during OBJ read
	
	return estab->list[reference_number];
}


static void add_symbol_to_esdict(Dict *dict, const char *symbol, int address) {
	int *value = malloc(sizeof(int));
	*value = address;
	add_to_dict(dict, symbol, (void*) value);
}

#ifdef TEST
int main(void) {
	Estab *estab = new_estab();

	assert(add_to_estab(estab, "Hello", 1, 1024) == 1);
	assert(add_to_estab(estab, "Ho", 2, 2048) == 1);
	assert(find_from_estab_by_symbol(estab, "Hello") == 1024);
	assert(find_from_estab_by_reference_number(estab, 1) == 1024);
	assert(find_from_estab_by_symbol(estab, "Ho") == 2048);
	assert(find_from_estab_by_reference_number(estab, 2) == 2048);
	assert(find_from_estab_by_symbol(estab, "Hollo") == -1);
	assert(find_from_estab_by_reference_number(estab, 3) == -1);

	assert(assign_reference_number_to_symbol(estab, "Hello", 3) == true);
	assert(find_from_estab_by_reference_number(estab, 3) == 1024);

	printf("----------------------------------------\n");
	printf("Automatic tests successful!\n");

	return 0;
}
#endif
