#pragma once
/*
 * interpreter.h
 *
 * This module is provides a clean interface to interpret commands
 * (ParsedCommand's) and run the appropriate function to execute that command.
 * 
 * Usage
 * Create a new Interpreter and add interpretable operations (OperationNode's)
 * to that Interpreter. Each operation has an operator string, the number
 * of arguments for that operation, and the function to call to execute that
 * operation.
 *
 * IMPORTANT: The signature of the function must be
 * int f(FILE *out, ProcessedCommand *pc);
 * This will not be explicitly checked during complication nor run-time.
 *
 * Once you've set up the Interpreter, you can let the Interpreter interpret 
 * the user's command (converted to ParsedCommand). Just call the function
 * interpret(Interpreter ip, ParsedCommand pc), and the appropriate function
 * will be called with the user's arguments.
 */
#include <stdbool.h>

#include "parser.h"
#include "generic_list.h"

typedef struct _Interpreter {
	List *operations;
	List *history;
	FILE *output_stream;
} Interpreter;

Interpreter *new_interpreter(FILE *out);
/*
 * Allocates and returns a new interpreter.
 */

void free_interpreter(Interpreter *ip);
/*
 * Deallocates the interpreter and all of the statements assigned to it.
 */

void add_operation(Interpreter *ip, char *operator, 
		           int argument_count, void *function);
/*
 * Add an operation to interpret with *ip*. For example, add <"command",
 * 2, func> to *ip*, and the func function will be called when the user
 * inputs something like "command argument1 argument2".
 */

bool interpret(Interpreter *ip, ParsedCommand *pc);
/*
 * Pass the user input to *ip* as a ParsedCommand *pc* and the interpretor
 * will 'magically' interpret that command and call the function within
 * the corresponding operation.
 *
 * Return FALSE is there is no such operation.
 */

bool interpret_and_free(Interpreter *ip, ParsedCommand *pc);
/*
 * Call interpret with *ip* and *pc* and free *pc* - since there is usually
 * no need to use it after it has been interpreted.
 */

void fprint_command_history(FILE *out, Interpreter *ip);
/*
 *  Print the list of commands that were successfully interpreted/executed by the interpeter
 */
