#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Recall these definitions
#define COMMAND_LENGTH 120

typedef struct _HistoryNode {
	struct _HistoryNode *next;
	char string[HISTORY_LENGTH];
	
} HistoryNode;

typedef struct _History {
	HistoryNode *head; // fake head
	HistoryNode *last;
} History;
*/

#ifdef TEST
// Manual unit test for this module
// Refer to the README for testing instructions
int main(void) {
	for (int i = 0; i < 20; ++i) {
		History *history = new_history();

		add_history(history, "Hello!");
		add_history(history, "Yo!");
		add_history(history, "Hello!");
		add_history(history, "Yo!");
		add_history(history, "Hello!");
		add_history(history, "Yo!");
		fprint_history(stdout, history);
		free_history(history);
	}

	return 0;
}
#endif

// Make sure to free after use!
History *new_history() {
	History *history = malloc(sizeof(History));
	history->head = malloc(sizeof(HistoryNode));
	history->head->next = NULL;
	history->last = history->head;

	return history;
}

void free_history(History *history) {
	HistoryNode *next;

	for (HistoryNode *node = history->head; node; node = next) {
		next = node->next;
		free(node);
	}

	free(history);
}


void add_history(History *history, char *string) {
	HistoryNode *new_node = malloc(sizeof(HistoryNode));
	strcpy(new_node->string, string);
	new_node->next = NULL;

	history->last->next = new_node;
	history->last = new_node;
}
	
void fprint_history(FILE *out, History *history) {
	int i = 1;
	for (HistoryNode *node = history->head->next; node; node = node->next, ++i)
		fprintf(out, "%d\t%s\n", i, node->string);
}

int has_history(History *history) {
	return history->head != history->last;
}
