#include "history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Recall these definitions
#define HISTORY_LENGTH 200

typedef struct _HistoryNode {
	struct _HistoryNode *next;
	char string[HISTORY_LENGTH];
	
} HistoryNode;

typedef struct _History {
	HistoryNode *head; // fake head
	HistoryNode *last;
} History;
*/

int main(void) {
	History *history = new_history();

	add_history(history, "Hello!");
	add_history(history, "Yo!");
	add_history(history, "Hello!");
	add_history(history, "Yo!");
	add_history(history, "Hello!");
	add_history(history, "Yo!");
	fprint_history(stdout, history);
	free_history(history);

	return 0;
}

History *new_history() {
	History *history = malloc(sizeof(History));
	history->head = malloc(sizeof(HistoryNode));
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

	history->last->next = new_node;
	history->last = new_node;
}
	
void fprint_history(FILE *out, History *history) {
	int i = 1;
	for (HistoryNode *node = history->head->next; node; node = node->next, ++i)
		fprintf(out, "%d\t%s\n", i, node->string);
}
