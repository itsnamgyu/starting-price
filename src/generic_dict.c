#include "generic_dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Recall these definitions
typedef struct _BucketNode {
	char *key;
	void *value;
	struct _BucketNode *link;
} BucketNode;

typedef struct _DictNode {
	BucketNode *head;
	BucketNode *last;
} DictNode;

typedef struct _Dict {
	int size;
	DictNode *nodes;
} Dict;
*/

#ifdef TEST
void print_string(FILE *out, void *string) {
	fprintf(out, "%s", (char*) string);
}

int main(void) {
	Dict *dict = new_dict(20);
	add_to_dict(dict, "hello", "Yeah, hi");
	add_to_dict(dict, "bye", "Cya");

	char *string;
	assert(find_from_dict(dict, "hello", (void*) &string));
	assert(!strcmp("Yeah, hi", string));
	assert(find_from_dict(dict, "bye", (void*) &string));
	assert(!strcmp("Cya", string));

	assert(dict_contains(dict, "hello"));
	assert(dict_contains(dict, "bye"));
	assert(!dict_contains(dict, "hoho"));
	assert(!dict_contains(dict, "Hello"));

	free_dict(dict, ignore_dict_values);

	printf("----------------------------------------\n");
	printf("Automatic tests successful!\n");

	{
		printf("\n");

		Dict *dict = new_dict(20);
		dict = new_dict(20);
		add_to_dict(dict, "hello", "yeah, hi");
		add_to_dict(dict, "bye", "Cya");

		printf("Print function\n");
		fprint_dict(stdout, dict, print_string);

		free_dict(dict, ignore_dict_values);
	}

	return 0;
}
#endif

Dict *new_dict(int size) {
	Dict *dict= malloc(sizeof(Dict));
	dict->size = size;
	dict->nodes = malloc(sizeof(DictNode) * size);
	
	for (int i = 0; i < size; ++i) {
		dict->nodes[i].head = malloc(sizeof(BucketNode));
		dict->nodes[i].head->key = NULL;
		dict->nodes[i].head->value = NULL;
		dict->nodes[i].head->link = NULL;
		dict->nodes[i].last = dict->nodes[i].head;
	}

	return dict;
}

static inline int hash(char *key, int table_size) {
	unsigned int hash_value = 0;
	for (; *key; ++key) {
		hash_value = hash_value << 1;
		hash_value ^= *key;
	}

	return hash_value % table_size;
}

void free_dict(Dict *dict, void (*free_value) (void *value)) {
	for (int i = 0; i < dict->size; ++i) {
		BucketNode *next;
		for (BucketNode *node = dict->nodes[i].head; node; node = next) {
			next = node->link;
			free_value(node->value);
			free(node->key);
			free(node);
		}
	}

	free(dict->nodes);
	free(dict);
}

void add_to_dict(Dict *dict, char *key, void *value) {
	int h = hash(key, dict->size);

	BucketNode *node = malloc(sizeof(BucketNode));
	node->key = malloc(sizeof(char) * (strlen(key) + 1));
	strcpy(node->key, key);
	node->link = NULL;
	node->value = value;

	dict->nodes[h].last->link = node;
	dict->nodes[h].last = node;
}

int find_from_dict(Dict *dict, char *key, void **value) {
	int h = hash(key, dict->size);
	
	for (BucketNode *node = dict->nodes[h].head->link; node; node = node->link)
		if (!(strcmp(key, node->key))) {
			*value = node->value;
			return 1;
		}

	return 0;
}

int dict_contains(Dict *dict, char *key) {
	int h = hash(key, dict->size);
	
	for (BucketNode *node = dict->nodes[h].head->link; node; node = node->link)
		if (!(strcmp(key, node->key))) {
			return 1;
		}

	return 0;
}

void fprint_dict(FILE *out, Dict *dict, void (*print_value) (FILE *out, void *value)) {
	for (int i = 0; i < dict->size; ++i) {
		fprintf(out, "%d : ", i);
		
		for (BucketNode *node = dict->nodes[i].head->link; node; node = node->link) {
			fprintf(out, "[%s,", node->key);
			print_value(out, node->value);
			fprintf(out, "]");
			if (node->link) fprintf(out, " -> ");
		}

		fprintf(out, "\n");
	}
}

void ignore_dict_values(void *value) {
	(void) value; // suppress unused warning
}
