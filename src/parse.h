#define PARSE_FORMAT_ERROR 1
#define TOO_MANY_ARGUMENTS_ERROR 2
#define COMMAND_TOO_LONG_ERROR 3
#define ARGUMENT_TOO_LONG_ERROR 4

#define COMMAND_LENGTH 120
#define TOKEN_COUNT 4
#define TOKEN_LENGTH 40

/*
 * Terminology
 * command: the original one-line string inputted by user
 * token: each word within command
 * 
 * Note that TOKEN_COUNT is the maximum number of arguments + 1
 */
typedef struct _ParsedCommand {
	char tokenized_command[COMMAND_LENGTH];
	char *operation;
	char *tokens[ARGUMENT_COUNT];
	int token_count;
} ParsedCommand;

ParsedCommand *parse_command(char *command, int *error_code);

void free_parsed_command(ParsedCommand *parsed);
