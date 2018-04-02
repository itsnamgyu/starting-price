#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "interpreter.h"

/* Note these declarations
typedef struct _OperationNode {
	char operator[OPERATION_LENGTH];
	int argument_count;
	void *function;
	struct _OperationNode *link;
} OperationNode;

typedef struct _Interpreter {
	OperationNode *head;
	OperationNode *last;
	FILE *output_stream;
} Interpreter;
*/

#ifdef TEST
#undef TEST
#include "parser.c"
int f1(FILE *out, ParsedCommand *pc) {
	return 1;
}

int f2(FILE *out, ParsedCommand *pc) {
	return 2;
}

int f3(FILE *out, ParsedCommand *pc) {
	return 3;
}

int main(void) {
	Interpreter *ip = new_interpreter(stdout);
	ParsedCommand *pc;
	int error_code;
	
	add_operation(ip, "hey", 0, f1);
	add_operation(ip, "ho", 1, f2);
	add_operation(ip, "ho", 3, f3);

	pc = parse_command("hey   \t", &error_code);
	assert(interpret_and_free(ip, pc) == 1);

	pc = parse_command("ho ajif    \t", &error_code);
	assert(interpret_and_free(ip, pc) == 2);
	
	pc = parse_command("ho   \t fds, \t d2", &error_code);
	assert(interpret_and_free(ip, pc) == 0);
	
	pc = parse_command("ho   \t fds, \t d2 , \t ji", &error_code);
	assert(interpret_and_free(ip, pc) == 3);
			
	printf("----------------------------------------\n");
	printf("Automatic tests successful!\n");

	free_interpreter(ip);

	return 0;
}
#endif

Interpreter *new_interpreter(FILE *out) {
	Interpreter *ip = malloc(sizeof(Interpreter));
	ip->head = malloc(sizeof(OperationNode));
	ip->last = ip->head;
	ip->head->link = NULL;
	ip->output_stream = out;
	
	return ip;
}

void free_interpreter(Interpreter *ip) {
	OperationNode *next;
	for (OperationNode *node = ip->head; node; node = next) {
		next = node->link;
		free(node);
	}
	free(ip);
}

void add_operation(Interpreter *ip, char *operator, 
		           int argument_count, void *function) {
	OperationNode *node = malloc(sizeof(OperationNode));
	strcpy(node->operator, operator);
	node->argument_count = argument_count;
	node->function = function;
	node->link = NULL;
	
	ip->last->link = node;
	ip->last = node;
}

int interpret(Interpreter *ip, ParsedCommand *pc) {
	// Assert that there were operations added to the interpreter
	assert (ip->head->link && "empty interpreter?");

	FILE *out = ip->output_stream;

	for (OperationNode *node = ip->head->link; node; node = node->link) {
		int match = 1;
		match = match && !strcmp(node->operator, pc->operator);
		match = match && (node->argument_count == pc->argument_count);
		
		if (match) return node->function(out, pc);
	}
	
	// Print error if there were no commands found
	fprintf(out, "error: no command %s that accepts %d argument%s.\n",
			pc->operator, pc->argument_count,
			pc->argument_count == 1 ? "" : "s");

	return 0;
}

int interpret_and_free(Interpreter *ip, ParsedCommand *pc) {
	int result = interpret(ip, pc);
	free(pc);

	return result;
}
