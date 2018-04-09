#ifndef GENERIC_LIST_H
#define GENERIC_LIST_H
#include <stdio.h>

typedef struct _LinkedNode {
	void *value;
	struct _LinkedNode *link;
} LinkedNode;

typedef struct _List {
	LinkedNode *head;
	LinkedNode *last;
} List;

List *new_list();

void add_to_list(List *list, void *value);

void free_list(List *list, void (*free_value) (void *value));

void fprint_list(FILE *out, List *list, void (*print_value) (FILE *out, void *value));

void ignore_list_values(void *value);
#endif
