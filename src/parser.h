#include "main.h"

#ifndef PARSER_H
#define PARSER_H

#define INVALID_FORMAT_ERROR 1
#define TOO_MANY_ARGUMENTS_ERROR 2
#define COMMAND_TOO_LONG_ERROR 3
#define ARGUMENT_TOO_LONG_ERROR 4

#define TOKEN_COUNT 4
#define ARGUMENT_COUNT 3
#define TOKEN_LENGTH 40

/*
 * Terminology
 * command: the original one-line string inputted by user
 * token: each word within command
 * operator: the actual "command word" part of the command
 * argument: the arguments to the operation. (all tokens excluding operation)
 * 
 * Note that TOKEN_COUNT is the maximum number of arguments + 1
 */
typedef struct _ParsedCommand {
	char original_command[COMMAND_LENGTH];
	char tokenized_command[COMMAND_LENGTH];
	char *operator;
	char *arguments[ARGUMENT_COUNT];
	int argument_count;
} ParsedCommand;

ParsedCommand *parse_command(char *command, int *error_code);

void free_parsed_command(ParsedCommand *parsed);
#endif 
