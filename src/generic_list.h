/*
 * generic_list
 *
 * This module provides a generic linked-list that stores values as
 * references, pointed by void pointers.
 *
 * ~~Because we have to implement like 15 frigging different linked-lists
 * during this project mann.~~
 *
 * To implement a linked-list that holds a custom type, just typedef List to
 * the list of that type (like typedef List StringList;) and wrap the 
 * following linked-list functions to handle that type. All of the linked-list
 * related gizmo's are implemented within this module!
 */

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
/*
 * Allocates and returns a new linked-list.
 */

void add_to_list(List *list, void *value);
/*
 * Adds a node containing *value* to *list*.
 */

void free_list(List *list, void (*free_value) (void *value));
/*
 * Frees the linked-list structure, and frees the invidiuals values of your
 * custom type using a custom free function (that you provide ;)
 */


void fprint_list(FILE *out, List *list, void (*print_value) (FILE *out, void *value));
/*
 * Print dictionary in the following format to the *out* stream.
 * VALUE -> VALUE -> ... -> VALUE\n
 * 
 * The VALUE part is printed using the print function *print_value*.
 * Yes, you provide that too :) Because, we don't know the type of *value!
 */

void ignore_list_values(void *value);
/*
 * Use this function in free_list when your values do not require freeing.
 * For example, references to statically allocated strings. This srsly doesn't
 * do anything with the passed *value*.
 */
#endif
