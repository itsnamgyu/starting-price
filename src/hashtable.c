#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Recall these definitions
#define HASH_TABLE_SIZE 20
#define HASH_KEY_LENGTH 100

typedef struct _Value {
	unsigned char opcode;
} Value;

typedef struct _BucketNode {
	char key[HASH_KEY_LENGTH];
	Value value;
	struct _BucketNode *next;
} BucketNode;

typedef struct _TableNode {
	BucketNode *head; // fake
	BucketNode *last;
} TableNode;

typedef struct _HashTable {
	TableNode nodes[HASH_TABLE_SIZE];
} HashTable;
*/

#ifdef TEST
// Test functions in the hashtable module
// Refer to the README on testing instructions
int main(void) {
	for (int i = 0; i < 10; ++i) {

		HashTable *table = new_hash_table();

		Value value;
		value.opcode = 12;
		add_to_hash_table(table, "Hello!", value);
		value.opcode = 23;
		add_to_hash_table(table, "Hoho!", value);

		for (int i = 0; i < 50; ++i) {
			char ho[3] = { 0 };
			ho[0] = 32 + i;
			ho[1] = 32 + i;
			Value value;
			value.opcode = i;
			add_to_hash_table(table, ho, value);
		}

		printf("Print hash table\n");
		fprint_hash_table(stdout, table);

		printf("\n");
		printf("Find non-exisitng\n");
		if (find_from_hash_table(table, "Non", &value))
			printf("> Found something..? FAIL\n");
		else 
			printf("> Yes! We couldn't find anything\n");

		printf("\n");
		printf("Find Hello!\n");
		if (find_from_hash_table(table, "Hello!", &value))
			printf("> Found this: %02X\n", value.opcode);
		else 
			printf("> Couldn't find anything..? FAIL\n");

		free_hash_table(table);

	}
	return 0;
}
#endif

HashTable *new_hash_table() {
	HashTable *table = malloc(sizeof(HashTable));
	
	for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
		table->nodes[i].head = malloc(sizeof(BucketNode));
		table->nodes[i].head->next = NULL;
		table->nodes[i].last = table->nodes[i].head;
	}

	return table;
}

static inline int hash(char *key) {
	unsigned int hash_value = 0;
	for (; *key; ++key) {
		hash_value = hash_value << 1;
		hash_value ^= *key;
	}

	return hash_value % HASH_TABLE_SIZE;
}

void free_hash_table(HashTable *table) {
	for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
		BucketNode *next;
		for (BucketNode *node = table->nodes[i].head; node; node = next) {
			next = node->next;
			free(node);
		}
	}

	free(table);
}

void add_to_hash_table(HashTable *table, char *key, Value value) {
	int h = hash(key);

	BucketNode *node = malloc(sizeof(BucketNode));
	strcpy(node->key, key);
	node->next = NULL;
	node->value = value;

	table->nodes[h].last->next = node;
	table->nodes[h].last = node;
}

int find_from_hash_table(HashTable *table, char *key, Value *value) {
	int h = hash(key);
	
	for (BucketNode *node = table->nodes[h].head->next; node; node = node->next)
		if (!(strcmp(key, node->key))) {
			*value = node->value;
			return 1;
		}

	return 0;
}

void fprint_hash_table(FILE *out, HashTable *table) {
	for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
		fprintf(out, "%d : ", i);
		
		for (BucketNode *node = table->nodes[i].head->next; node; node = node->next) {
			fprintf(out, "[%s,%02X]", node->key, node->value.opcode);
			if (node->next) fprintf(out, " -> ");
		}

		fprintf(out, "\n");
	}
}
