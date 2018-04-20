#pragma once
/*
 * generic_dict
 *
 * This module provides a generic dictionary (internally, a hash table) that
 * stores values as references, pointed by void pointers.
 *
 * To implement a dictionary that holds a custom type, just typedef Dict to
 * the dictionary of that type (like typedef Dict StringDict;) and wrap the 
 * following dictionary functions to handle that type. All of the hash table
 * related gizmo's are implemented within this module!
 */
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
/*
 * Allocates and returns a new dictionary with a bucket size of *size*
 */

void free_dict(Dict *dict, void (*free_value) (void *value));
/*
 * Frees the dictonary structure, and frees the invidiuals values of your
 * custom type using a custom free function (that you provide ;)
 */

void add_to_dict(Dict *dict, char *key, void *value);
/*
 * Adds a key-value pair to the dictionary.
 */

int find_from_dict(Dict *dict, char *key, void **value);
/*
 * Finds a value cooresponing to *key* within *dict*. If found, it is assigned
 * to *value.
 * 
 * Return success as boolean.
 */

int dict_contains(Dict *dict, char *key);
/*
 * Return whether if *key* exists within *dict*.
 */

void fprint_dict(FILE *out, Dict *dict, void (*print_value) (FILE *out, void *value));
/*
 * Print dictionary in the following format to the *out* stream.
 * 0: [KEY,VALUE] -> [KEY,VALUE]...
 * 
 * The VALUE part is printed using the print function *print_value*.
 * Yes, you provide that too :) Because, we don't know the type of *value!
 */

void fprint_dict_values(FILE *out, Dict *dict, 
		void (*print_value) (FILE *out, void *value));
/*
 * Print dictionary in the following format to the *out* stream.
 * \tKEY\tVALUE\n
 * \tKEY\tVALUE\n
 * ...
 * 
 * Again, the VALUE part is printed using the print function *print_value*.
 * Of course, you provide that too :)
 */

void ignore_dict_values(void *value);
/*
 * Use this function in free_dict when your values do not require freeing.
 * For example, references to statically allocated strings. This srsly doesn't
 * do anything with the passed *value*.
 */
