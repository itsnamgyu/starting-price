#include "utility.h"
#include "interpreter.h"
#include "generic_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct _Operation {
	char *operator;
	int argument_count;
	int (*function)(FILE*, ParsedCommand*);
} Operation;


inline static bool has_history(Interpreter *ip);
// Check if interpreter contains at least one history of a successful user command.

inline static bool is_history(ParsedCommand *pc);
// Check if the command is history or hi (w/o any arguments).

inline static bool these_match(Operation *op, ParsedCommand *pc);
// Check if the command these_match the operation

static Operation *new_operation(const char *operator, int argument_count, void *function);
// Allocates and intilized an Operation and returns the pointer.

static void free_operation(void *operation);
// Free an Operation.


Interpreter *new_interpreter(FILE *out) {
	Interpreter *ip = malloc(sizeof(Interpreter));
	ip->output_stream = out;
	ip->operations = new_list();
	ip->history = new_list();
	
	return ip;
}

void free_interpreter(Interpreter *ip) {
	free_list(ip->operations, free_operation);
	free_list(ip->history, free);
	free(ip);
}

void add_operation(Interpreter *ip, char *operator, int argument_count, void *function) {
	Operation *operation = new_operation(operator, argument_count, function);
	add_to_list(ip->operations, operation);
}

bool interpret(Interpreter *ip, ParsedCommand *pc) {
	char *empty_msg = "warning: interpretting from an empty interpreter";
	if (list_empty(ip->operations)) fprintf(stderr, "%s\n", empty_msg);

	FILE *out = ip->output_stream;

	if (is_history(pc)) {
		if (has_history(ip)) {
			add_to_list(ip->history, malloc_strcpy(pc->original_command));
			fprint_command_history(out, ip);
			return true;
		} else {
			add_to_list(ip->history, malloc_strcpy(pc->original_command));
			return false;
		}
	}

	// if found command
	for (LinkedNode *node = ip->operations->head->link; node; node = node->link) {
		Operation *op = (Operation*) node->value;
		if (these_match(op, pc)) {
			bool run = op->function(out, pc);
			if (run) add_to_list(ip->history, malloc_strcpy(pc->original_command));
			return run;
		}
	}
	
	// if no commands found -> print error
	fprintf(out, "error: no command %s that accepts %d argument%s.\n",
			pc->operator, pc->argument_count,
			pc->argument_count == 1 ? "" : "s");

	return false;
}

void fprint_command_history(FILE *out, Interpreter *ip) {
	int i = 1;
	for (LinkedNode *node = ip->history->head->link; node; node = node->link, ++i)
		fprintf(out, "%d\t%s\n", i, node->value);
}

bool interpret_and_free(Interpreter *ip, ParsedCommand *pc) {
	bool run = interpret(ip, pc);
	free(pc);
	return run;
}


inline static bool has_history(Interpreter *ip) {
	return !list_empty(ip->history);
}

inline static bool these_match(Operation *op, ParsedCommand *pc) {
	bool match = true;
	match = match && !strcmp(op->operator, pc->operator);
	match = match && (op->argument_count == pc->argument_count);
	return match;
}

inline static bool is_history(ParsedCommand *pc) {
	bool match = false;
	match = match || !strcmp(pc->operator, "history");
	match = match || !strcmp(pc->operator, "hi");
	match = match && pc->argument_count == 0;
	return match;
}

static Operation *new_operation(const char *operator, int argument_count, void *function) {
	assert(strlen(operator) < TOKEN_LENGTH);

	char *operator_copy = malloc_strcpy(operator);
	assert(operator_copy != NULL);

	Operation *op = malloc(sizeof(Operation));
	op->operator = operator_copy;
	op->argument_count = argument_count;
	op->function = function;

	return op;
}

static void free_operation(void *_op) {
	Operation *op = (Operation*) _op;
	free(op->operator);
	free(op);
}


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
