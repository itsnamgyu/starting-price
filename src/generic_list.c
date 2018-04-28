#define GENERIC_LIST_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "generic_list.h"

List *new_list() {
	List *list = malloc(sizeof(List));
	LinkedNode *node = malloc(sizeof(LinkedNode));
	node->link = NULL;
	node->value = NULL;

	list->head = node;
	list->last = list->head;

	return list;
}

void add_to_list(List *list, void *value) {
	LinkedNode *node = malloc(sizeof(LinkedNode));
	node->value = value;
	node->link = NULL;
	list->last->link = node;
	list->last = node;
}

int list_empty(List *list) {
	return (list->head->link == NULL);
}

void free_list(List *list, void (*free_value) (void *value)) {
	LinkedNode *next;

	for (LinkedNode *node = list->head; node; node = next) {
		next = node->link;
		if (node != list->head) free_value(node->value);
		free(node);
	}
}

void fprint_list(FILE *out, List *list, void (*print_value) (FILE *out, void *value)) {
	for (LinkedNode *node = list->head->link; node; node = node->link) {
		print_value(out, node->value);
		if (node->link)
			printf(" -> ");
	}
	printf("\n");
}

void ignore_list_values(void *value) {
	// suppress unused warning
	(void) value;
}

#ifdef TEST
void print_string(FILE *out, void *string) {
	fprintf(out, "%s", (char*) string);
}

int main(void) {
	List *list = new_list();

	add_to_list(list, "Hello");
	add_to_list(list, "Yo");

	assert(!strcmp(list->head->link->value, "Hello"));
	assert(!strcmp(list->head->link->link->value, "Yo"));

	free_list(list, ignore_list_values);

	printf("----------------------------------------\n");
	printf("Automatic tests successful!\n");

	{
		printf("\n");

		List *list = new_list();

		add_to_list(list, "Hello");
		add_to_list(list, "Yo");

		fprint_list(stdout, list, print_string);

		free_list(list, ignore_list_values);
	}

	return 0;
}
#endif
