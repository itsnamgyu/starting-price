#include <stdio.h>

#define HASH_TABLE_SIZE 20
#define HASH_KEY_LENGTH 100

#ifndef HASHTABLE_H
#define HASHTABLE_H
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
/*	Description
 *	Initiate memory for a valid HashTable.
 */

void free_hash_table(HashTable *table);
/*	Description
 *  Free memory and destory HashTable.
 */

void add_to_hash_table(HashTable *table, char *key, Value value);
/*	Description
 *  Add a key-value entry to HashTable.
 *
 *  Arguments
 *  char *key: key as a string.
 *  Value value: the value to store. To change the fields of the value later
 *  on, change the definition of struct Value
 */

int find_from_hash_table(HashTable *table, char *key, Value *value);
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
#endif
