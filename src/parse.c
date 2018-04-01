#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Note these definitions
#define INVALID_FORMAT_ERROR 1
#define TOO_MANY_ARGUMENTS_ERROR 2
#define COMMAND_TOO_LONG_ERROR 3
#define ARGUMENT_TOO_LONG_ERROR 4

#define COMMAND_LENGTH 120
#define TOKEN_COUNT 4
#define ARGUMENT_COUNT 3
#define TOKEN_LENGTH 40

typedef struct _ParsedCommand {
	char tokenized_command[COMMAND_LENGTH];
	char *operator;
	char *arguments[ARGUMENT_COUNT];
	int argument_count;
} ParsedCommand;
*/

static inline int is_comma(char c);
// Return whether c is a comma. Functionalized for readability.

static inline int is_normal_letter(char c);
// Return whether c is a normal letter (not comma, space, newline, or tab).

static int validate_and_tokenize(char *string, int *token_count, char **tokens);	
// Return whether the comma sequence is valid and remove commas.

static int validate_and_remove_commas(char *string);
// Return whether the spacing and number of command tokens are valid and
// save the (number of) tokens to token(s)(_count).

#ifdef TEST
int main(void) {
	ParsedCommand *pc;
	int error_code;
	
	pc = NULL;
	pc = parse_command("ho    hoho,   koko,     mo", &error_code);
	assert(pc && "command should be valid");

	pc = NULL;
	pc = parse_command("comma is not here!", &error_code);
	assert(!pc && "command shouldn't be valid");
	assert(error_code == INVALID_FORMAT_ERROR);

	pc = NULL;
	pc = parse_command("comma is, not there!", &error_code);
	assert(!pc && "command shouldn't be valid");
	assert(error_code == INVALID_FORMAT_ERROR);

	pc = NULL;
	pc = parse_command("too many, arguments, you, know", &error_code);
	assert(!pc && "command shouldn't be valid");
	assert(error_code == TOO_MANY_ARGUMENTS_ERROR);

	pc = NULL;
	pc = parse_command("yolo \t ho, \t ko, \t yo ", &error_code);
	assert(pc && "command should be valid");
	assert(!strcmp(pc->operator, "yolo"));
	assert(!strcmp(pc->arguments[0], "ho"));
	assert(!strcmp(pc->arguments[1], "ko"));
	assert(!strcmp(pc->arguments[2], "yo"));

	printf("Automatic tests successful!\n");

	char *test_string = "yolo \t ho, \t ho, \t yo";
	printf("\nParsing %s\n", test_string);
	pc = parse_command("yolo \t ho, \t ko, \t yo ", &error_code);
	printf("operator: %s\n", pc->operator);
	for (int i = 0; i < pc->argument_count; ++i)
		printf("argument%d: %s\n", i, pc->arguments[i]);

	return 0;
}
#endif

ParsedCommand *parse_command(char *command, int *error_code) {
	ParsedCommand *pc = malloc(sizeof(ParsedCommand));
	strcpy(pc->tokenized_command, command);

	if (!validate_and_remove_commas(pc->tokenized_command)) {
		free(pc); *error_code = INVALID_FORMAT_ERROR; return NULL;
	}

	// Get all tokens and assign to operator + arguments within pc
	int token_count;
	char *tokens[TOKEN_COUNT];
	if (!validate_and_tokenize(pc->tokenized_command, &token_count, tokens)) {
		free(pc); *error_code = TOO_MANY_ARGUMENTS_ERROR; return NULL;
	}
	pc->argument_count = token_count - 1;
	pc->operator = tokens[0];
	for (int i = 1; i < token_count; ++i)
		pc->arguments[i - 1] = tokens[i];

	return pc;
}

static inline int is_comma(char c) {
	return c == ',';
}

static inline int is_normal_letter(char c) {
	if (c == ' ') return 0;
	if (c == '\t') return 0;
	if (c == ',') return 0;
	if (c == '\n') return 0;

	return 1;
}

static int validate_and_remove_commas(char *string) {
	int word_index = 0;
	int was_letter = 0;
	int comma_avail = 0;

	for (; *string; ++string) {
		if (is_normal_letter(*string)) {
			if (!was_letter) {
				word_index++;
				if (comma_avail > 0) return 0;
				if (word_index >= 2) comma_avail = 1;
			}
			was_letter = 1;
		} else {
			was_letter = 0;
			if (is_comma(*string)) {
				*string = ' ';
				comma_avail--;
			}
			if (comma_avail < 0) return 0;
		}
	}
	
	return 1;
}

static int validate_and_tokenize(char *string, int *token_count, char **tokens) {
	*token_count = 0;

	int waiting_for_word = 1;
	for (; *string; ++string) {
		if (is_normal_letter(*string)) {
			if (waiting_for_word)  {
				if (*token_count == TOKEN_COUNT) return 0;
				tokens[(*token_count)++] = string;
				waiting_for_word = 0;
			}
		} else {
			*string = '\0';
			waiting_for_word = 1;
		}
	}

	return 1;
}
