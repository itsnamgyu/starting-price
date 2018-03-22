#include <stdio.h>

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

HashTable *new_hash_table();

void free_hash_table(HashTable *table);

void add_to_hash_table(HashTable *table, char *key, Value value);

int find_from_hash_table(HashTable *table, char *key, Value *value);

void fprint_hash_table(FILE *out, HashTable *table);
