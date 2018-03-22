#include <stdio.h>

#define COMMAND_LENGTH 200

typedef struct _HistoryNode {
	struct _HistoryNode *next;
	char string[COMMAND_LENGTH];
	
} HistoryNode;

typedef struct _History {
	HistoryNode *head; // fake head
	HistoryNode *last;
} History;

History *new_history();

void free_history(History *history);

void add_history(History *history, char *string);
	
void fprint_history(FILE *out, History *history);

int has_history(History *history);
