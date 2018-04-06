#include <stdio.h>
#include <stdlib.h>

#define Type Int
#define type int
#define ValueType int
#include "linked_list.gh"

int main(void) {
	IntList *list = new_int_list();
	
	int *v = malloc(sizeof(int));
	*v = 23;
	add_int_to(list, v);
	v = malloc(sizeof(int));
	*v = 1244;
	add_int_to(list, v);

	for (IntNode *node = list->head->link; node; node = node->link) {
		printf("%d\n", *node->value);
	};

	free_int_list(list);
}

void free_int(int *n) {
	free(n);
}

#define free_value_function free_int
#define Type Int
#define type int
#include "linked_list.gc"
