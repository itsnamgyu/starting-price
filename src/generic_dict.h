#ifndef GENERIC_DICT_H
#define GENERIC_DICT_H
#include <stdio.h>

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

Dict *new_dict(int size);

void free_dict(Dict *dict, void (*free_value) (void *value));

void add_to_dict(Dict *dict, char *key, void *value);

int find_from_dict(Dict *dict, char *key, void **value);

int dict_contains(Dict *dict, char *key);

void fprint_dict(FILE *out, Dict *dict, void (*print_value) (FILE *out, void *value));

void fprint_dict_values(FILE *out, Dict *dict, 
		void (*print_value) (FILE *out, void *value));

void ignore_dict_values(void *value);
#endif
