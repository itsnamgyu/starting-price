#include "dir.h"
#include "hashtable.h"
#include "help.h"
#include "history.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ERROR_MESSAGE_LENGTH 2000
#define MAX_TOKENS 4
#define OPCODE_FILE "opcode.txt"
#define PROMPT "sicsim> "

int is_comma(char c);

int hex_to_uint(char *string, unsigned int *value);

int is_normal_letter(char c);

int validate_and_remove_commas(char *string);

int validate_and_tokenize(char *string, int *token_count, char **tokens);

int is_command(char *token0, char *command, char *alias);

int get_line(char *string);

void load_hash_table(HashTable *table, FILE *in);

int main(void) {
	History *history = new_history();
	HashTable *table = new_hash_table();
	Block *block = new_memory_block();

	char command[COMMAND_LENGTH];
	char processed_command[COMMAND_LENGTH];

	FILE *opcode_in;
	if (!(opcode_in = fopen(OPCODE_FILE, "r")))
		printf("error opening opcode.txt. Continuing without opcodes\n");
	else {
		load_hash_table(table, opcode_in);
		fclose(opcode_in);
	}
	
	while (1) {
		printf("%s", PROMPT);
		if (!get_line(command)) { 
			printf("error: command is too long\n"); 
			continue;
		}

		strcpy(processed_command, command);

		if (!validate_and_remove_commas(processed_command)) {
			printf("error: command not parsable\n"); continue;
		}

		int token_count;
		char *tokens[MAX_TOKENS];
		if (!validate_and_tokenize(processed_command, &token_count, tokens)) {
			printf("error: too many arguments\n"); continue;
		}


		if (is_command(tokens[0], "quit", "q"))
			return 0;

		if (is_command(tokens[0], "history", "hi")) {
			if (token_count != 1) {
				printf("error: history does not take any arguments\n");
				continue;
			}
			int print = has_history(history);
			add_history(history, command);
			if (print) fprint_history(stdout, history);
			continue;
		}
		
		if (is_command(tokens[0], "help", "h")) {
			if (token_count != 1) {
				printf("error: help does not take any arguments\n");
				continue;
			}
			fprint_help(stdout);

		} else if (is_command(tokens[0], "dir", "d")) {
			if (token_count != 1) {
				printf("error: dir does not take any arguments\n"); 
				continue;
			}
			fprint_dir(stdout);

		} else if (is_command(tokens[0], "dump", "du")) {
			int start = -1;
			int end = -1;
			if (token_count == 4) {
				printf("error: dump takes up to 2 arguments\n");
				continue; 
			}
			if (token_count >= 2) {
				unsigned int value;
				if (!hex_to_uint(tokens[1], &value) || value >= BLOCK_SIZE) {
					printf("error: invalid start address\n");
					continue;
				}
				start = value;
			}
			if (token_count >= 3) {
				unsigned int value;
				if (!hex_to_uint(tokens[2], &value) || value >= BLOCK_SIZE || value < start) {
					printf("error: invalid end address\n");
					continue;
				}
				end = value;
			}
			
			char *string = dump_memory(block, start, end);
			printf("%s", string);
			free(string);

		} else if (is_command(tokens[0], "edit", "e")) {
			if (token_count != 3) {
				printf("error: edit takes exactly 2 arguments\n");
				continue;
			}
			unsigned int address;
			if (!hex_to_uint(tokens[1], &address) || address >= BLOCK_SIZE) {
				printf("error: invalid address\n");
				continue;
			}
			unsigned int value;
			if (!hex_to_uint(tokens[2], &value) || value >= 256) {
				printf("error: invalid value\n");
				continue;
			}
			set_memory(block, address, value);

		} else if (is_command(tokens[0], "fill", "f")) {
			if (token_count != 4) {
				printf("error: fill takes exactly 3 arguments\n");
				continue;
			}

			unsigned int start;
			if (!hex_to_uint(tokens[1], &start) || start >= BLOCK_SIZE) {
				printf("error: invalid start address\n"); continue;
			}
			unsigned int end;
			if (!hex_to_uint(tokens[2], &end) || end >= BLOCK_SIZE || end < start) {
				printf("error: invalid end address\n"); continue;
			}
			unsigned int value;
			if (!hex_to_uint(tokens[3], &value) || value >= 256) {
				printf("error: invalid value\n"); continue;
			}
			fill_memory(block, start, end ,value);

		} else if (!strcmp(tokens[0], "reset")) {
			reset_memory(block);

		} else if (!strcmp(tokens[0], "opcode")) {
			if (token_count != 2) {
				printf("error: opcode takes exactly 1 argument\n");
				continue;
			}

			Value value;
			if (!find_from_hash_table(table, tokens[1], &value)) {
				printf("Couldn't find opcode for %s\n", tokens[1]); continue;
	 		}  else {
				printf("opcode is %02X\n", value.opcode);
			}

		} else if (!strcmp(tokens[0], "opcodelist")) {
			if (token_count != 1) { 
				printf("error: opcodelist does not take any arguments\n");
				continue;
			}
			fprint_hash_table(stdout, table);

		} else {
			printf("error: no such command\n");
			continue;
		}

		add_history(history, command);
	}

	return 0;
}

int is_comma(char c) {
	return c == ',';
}

int hex_to_uint(char *string, unsigned int *value) {
	int length = strlen(string);
	char format_string[20];
	sprintf(format_string, "%%%dX", length);
	if (sscanf(string, format_string, value) == 1) return 1;
	else return 0;
}

int is_normal_letter(char c) {
	if (c == ' ') return 0;
	if (c == '\t') return 0;
	if (c == ',') return 0;
	if (c == '\n') return 0;

	return 1;
}

int validate_and_remove_commas(char *string) {
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

int validate_and_tokenize(char *string, int *token_count, char **tokens) {
	*token_count = 0;

	int waiting_for_word = 1;
	for (; *string; ++string) {
		if (is_normal_letter(*string)) {
			if (waiting_for_word)  {
				tokens[(*token_count)++] = string;
				if (*token_count > MAX_TOKENS) return 0;
				waiting_for_word = 0;
			}
		} else {
			*string = '\0';
			waiting_for_word = 1;
		}
	}

	return 1;
}

int is_command(char *token0, char *command, char *alias) {
	if (!strcmp(token0, command)) return 1;
	if (!strcmp(token0, alias)) return 1;
	return 0;
}

int get_line(char *string) {
	char *p = string;

	do {
		if (p - string == COMMAND_LENGTH - 1) return 0;
		*p = getchar();
	} while (*(p++) != '\n');

	*(p - 1) = 0; // places null-char at \n

	return 1;
}

void load_hash_table(HashTable *table, FILE *in) {
	unsigned int opcode;
	char mnemonic[100];
	char operand_count[100];
	while (fscanf(in, "%02X %s %s", &opcode, mnemonic, operand_count) == 3) {
		assert(opcode < 256);
		Value value;
		value.opcode = opcode;
		add_to_hash_table(table, mnemonic, value);
	}
}
