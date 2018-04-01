#include "main.h"
#include "file.h"
#include "hashtable.h"
#include "help.h"
#include "history.h"
#include "memory.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define OPCODE_FILE "opcode.txt"
#define PROMPT "sicsim> "

static inline int hex_to_uint(char *string, unsigned int *value);
// Convert hex string to uint and save to value. Return success as boolean.

static inline int is_command(char *token0, char *command, char *alias);
// Return whether the given command tokens coorespond to the given command
// or alias.

static inline int get_line(char *string);
// Scan one line to string and return whether it is the appropriate length.

void load_hash_table(HashTable *table, FILE *in);
// Load HashTable for the given opcode input file and save to table.

int main(void) {
	History *history = new_history();
	HashTable *table = new_hash_table();
	Block *block = new_memory_block();

	FILE *opcode_in;
	if (!(opcode_in = fopen(OPCODE_FILE, "r")))
		printf("error opening opcode.txt. Continuing without opcodes\n");
	else {
		load_hash_table(table, opcode_in);
		fclose(opcode_in);
	}
	
	char command[COMMAND_LENGTH];
	ParsedCommand *pc;

	while (1) {
		printf("%s", PROMPT);
		if (!get_line(command)) { 
			printf("error: command is too long\n"); 
			continue;
		}

		int error_code;
		if (!(pc = parse_command(command, &error_code))) {
			switch (error_code) {
				case TOO_MANY_ARGUMENTS_ERROR:
					printf("error: too many arguments\n"); break;
				case INVALID_FORMAT_ERROR:
					printf("error: format is invalid\n"); break;
				default:
					printf("error: could not parse command\n");
					break;
			}

			continue;
		}

		if (is_command(pc->operator, "quit", "q"))
			return 0;

		if (is_command(pc->operator, "history", "hi")) {
			if (pc->argument_count != 0) {
				printf("error: history does not take any arguments\n");
				continue;
			}
			int print = has_history(history);
			add_history(history, command);
			if (print) fprint_history(stdout, history);
			continue;
		}
		
		if (is_command(pc->operator, "help", "h")) {
			if (pc->argument_count != 0) {
				printf("error: help does not take any arguments\n");
				continue;
			}
			fprint_help(stdout);

		} else if (is_command(pc->operator, "dir", "d")) {
			if (pc->argument_count != 0) {
				printf("error: dir does not take any arguments\n"); 
				continue;
			}
			fprint_dir(stdout);

		} else if (is_command(pc->operator, "dump", "du")) {
			int start = -1;
			int end = -1;
			if (pc->argument_count == 3) {
				printf("error: dump takes up to 2 arguments\n");
				continue; 
			}
			if (pc->argument_count >= 1) {
				unsigned int value;
				if (!hex_to_uint(pc->arguments[0], &value) || value >= BLOCK_SIZE) {
					printf("error: invalid start address\n");
					continue;
				}
				start = value;
			}
			if (pc->argument_count >= 2) {
				unsigned int value;
				if (!hex_to_uint(pc->arguments[1], &value) || value >= BLOCK_SIZE || value < start) {
					printf("error: invalid end address\n");
					continue;
				}
				end = value;
			}
			
			char *string = dump_memory(block, start, end);
			printf("%s", string);
			free(string);

		} else if (is_command(pc->operator, "edit", "e")) {
			if (pc->argument_count != 2) {
				printf("error: edit takes exactly 2 arguments\n");
				continue;
			}
			unsigned int address;
			if (!hex_to_uint(pc->arguments[0], &address) || address >= BLOCK_SIZE) {
				printf("error: invalid address\n");
				continue;
			}
			unsigned int value;
			if (!hex_to_uint(pc->arguments[1], &value) || value >= 256) {
				printf("error: invalid value\n");
				continue;
			}
			set_memory(block, address, value);

		} else if (is_command(pc->operator, "fill", "f")) {
			if (pc->argument_count != 3) {
				printf("error: fill takes exactly 3 arguments\n");
				continue;
			}

			unsigned int start;
			if (!hex_to_uint(pc->arguments[0], &start) || start >= BLOCK_SIZE) {
				printf("error: invalid start address\n"); continue;
			}
			unsigned int end;
			if (!hex_to_uint(pc->arguments[1], &end) || end >= BLOCK_SIZE || end < start) {
				printf("error: invalid end address\n"); continue;
			}
			unsigned int value;
			if (!hex_to_uint(pc->arguments[2], &value) || value >= 256) {
				printf("error: invalid value\n"); continue;
			}
			fill_memory(block, start, end ,value);

		} else if (!strcmp(pc->operator, "reset")) {
			reset_memory(block);

			if (pc->argument_count != 0) {
				printf("error: reset does not take any arguments\n"); 
				continue;
			}
		} else if (!strcmp(pc->operator, "opcode")) {
			if (pc->argument_count != 1) {
				printf("error: opcode takes exactly 1 argument\n");
				continue;
			}

			Value value;
			if (!find_from_hash_table(table, pc->arguments[0], &value)) {
				printf("Couldn't find opcode for %s\n", pc->arguments[0]); continue;
	 		}  else {
				printf("opcode is %02X\n", value.opcode);
			}

		} else if (!strcmp(pc->operator, "opcodelist")) {
			if (pc->argument_count != 0) { 
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

	free(block);
	free_history(history);
	free_hash_table(table);

	return 0;
}

static inline int hex_to_uint(char *string, unsigned int *value) {
	int length = strlen(string);
	char format_string[20];
	sprintf(format_string, "%%%dX", length);
	if (sscanf(string, format_string, value) == 1) return 1;
	else return 0;
}

static inline int is_command(char *token0, char *command, char *alias) {
	if (!strcmp(token0, command)) return 1;
	if (!strcmp(token0, alias)) return 1;
	return 0;
}

static inline int get_line(char *string) {
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
