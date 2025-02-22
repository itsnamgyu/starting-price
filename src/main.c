#define MAIN_C
#include "main.h"
#include "file.h"
#include "hashtable.h"
#include "help.h"
#include "memory.h"
#include "parser.h"
#include "interpreter.h"
#include "global.h"
#include "assemble.h"
#include "estab.h"
#include "register.h"
#include "run.h"
#include "loader.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define OPCODE_FILE "opcode.txt"
#define RESERVED_FILE "reserved.txt"
#define PROMPT "sicsim> "

static inline int hex_to_uint(char *string, unsigned int *value);
// Convert hex string to uint and save to value. Return success as boolean.

static inline int get_line(char *string);
// Scan one line to string and return whether it is the appropriate length.

static void load_hash_table(HashTable *table, FILE *in);
// Load HashTable for the given opcode input file and save to table.

static void load_reserved_dict(ReservedDict *dict, FILE *in);

static int quit_0(FILE *out, ParsedCommand *pc);

static int dir_0(FILE *out, ParsedCommand *pc);

static int dump_0(FILE *out, ParsedCommand *pc);

static int dump_1(FILE *out, ParsedCommand *pc);

static int dump_2(FILE *out, ParsedCommand *pc);

static int edit_2(FILE *out, ParsedCommand *pc);

static int fill_3(FILE *out, ParsedCommand *pc);

static int help_0(FILE *out, ParsedCommand *pc);

static int reset_0(FILE *out, ParsedCommand *pc);

static int opcodelist_0(FILE *out, ParsedCommand *pc);

static int opcode_1(FILE *out, ParsedCommand *pc);

static int type_1(FILE *out, ParsedCommand *pc);

static int bp_1(FILE *out, ParsedCommand *pc);

static int bp_0(FILE *out, ParsedCommand *pc);

static int progaddr_1(FILE *out, ParsedCommand *pc);

static int run_0(FILE *out, ParsedCommand *pc);

Global G;

int main(void) {
	G.table = new_hash_table();
	G.block = new_memory_block();
	G.reserved = new_reserved_dict();

	FILE *opcode_in;
	if (!(opcode_in = fopen(OPCODE_FILE, "r")))
		printf("error opening %s. continuing without opcodes\n", OPCODE_FILE);
	else {
		load_hash_table(G.table, opcode_in);
		fclose(opcode_in);
	}

	FILE *reserved_in;
	if (!(reserved_in = fopen(RESERVED_FILE, "r")))
		printf("error opening %s. continuing without reserved tokens\n", RESERVED_FILE);
	else {
		load_reserved_dict(G.reserved, reserved_in);
		fclose(reserved_in);
	}

	Interpreter *ip = new_interpreter(stdout);
	add_operation(ip, "q", 0, quit_0);
	add_operation(ip, "quit", 0, quit_0);
	add_operation(ip, "dir", 0, dir_0);
	add_operation(ip, "d", 0, dir_0);
	add_operation(ip, "edit", 2, edit_2);
	add_operation(ip, "e", 2, edit_2);
	add_operation(ip, "du", 0, dump_0);
	add_operation(ip, "du", 1, dump_1);
	add_operation(ip, "du", 2, dump_2);
	add_operation(ip, "dump", 0, dump_0);
	add_operation(ip, "dump", 1, dump_1);
	add_operation(ip, "dump", 2, dump_2);
	add_operation(ip, "help", 0, help_0);
	add_operation(ip, "h", 0, help_0);
	add_operation(ip, "f", 3, fill_3);
	add_operation(ip, "fill", 3, fill_3);
	add_operation(ip, "reset", 0, reset_0);
	add_operation(ip, "opcodelist", 0, opcodelist_0);
	add_operation(ip, "opcode", 1, opcode_1);
	add_operation(ip, "type", 1, type_1);
	add_operation(ip, "assemble", 1, assemble_1);
	add_operation(ip, "symbol", 0, symbol_0);
	add_operation(ip, "bp", 0, bp_0);
	add_operation(ip, "bp", 1, bp_1);
	add_operation(ip, "progaddr", 1, progaddr_1);
	add_operation(ip, "run", 0, run_0);

	char command[COMMAND_LENGTH];
	ParsedCommand *pc;
	while (1) {
		printf("%s", PROMPT);
		if (!get_line(command)) { 
			printf("error: command is too long\n"); 
			continue;
		}
		if (!*command) continue;

		int error_code;
		// exception for loader
		char filenames[3][100];
		FILE *files[3];
		int res = sscanf(command, "loader %s %s %s", filenames[0], filenames[1], filenames[2]);
		if (res > 0 ) {
			for (int i = 0; i < res; ++i) {
				if (!(files[i] = fopen(filenames[i], "r"))) {
					printf("could not open file %s\n", filenames[0]);
					continue;
				}
			}

			if (load(files, res, G.block))
				add_to_history(ip, command);
			continue;
		}


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
		
		interpret(ip, pc);
		free(pc);
	}

	free_interpreter(ip);
	free_hash_table(G.table);
	free(G.block);
	free_reserved_dict(G.reserved);

	return 0;
}

static inline int hex_to_uint(char *string, unsigned int *value) {
	int length = strlen(string);
	char format_string[20];
	int used;

	sprintf(format_string, "%%%dX%%n", length);

	/* Note that %n assigns the number of characters used for sscanf.
	 * Also note that sscanf returns the number of fields whose values were
	 * assigned - excluding %n
	 */
	return sscanf(string, format_string, value, &used) && used == length;
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

static void load_hash_table(HashTable *table, FILE *in) {
	unsigned int opcode;
	char mnemonic[100];
	char operand_count[100];
	while (fscanf(in, "%02X %s %s", &opcode, mnemonic, operand_count) == 3) {
		assert(opcode < 256);
		add_to_hash_table(table, mnemonic, opcode);
	}
}

static void load_reserved_dict(ReservedDict *dict, FILE *in) {
	char string[100];
	int type;
	while (fscanf(in, "%s %d", string, &type) == 2) {
		add_to_reserved_dict(dict, string, (ReservedType) type);
	}
}

static int quit_0(FILE *out, ParsedCommand *pc) {
	free(G.block);
	free_hash_table(G.table);

	exit(0);
	return 1;
}

static int dir_0(FILE *out, ParsedCommand *pc) {
	fprint_dir(out);
	return 1;
}

static int dump_0(FILE *out, ParsedCommand *pc) {
	dump_memory(out, G.block, -1, -1);
	return 1;
}

static int dump_1(FILE *out, ParsedCommand *pc) {
	unsigned int start;
	if (!hex_to_uint(pc->arguments[0], &start) || start >= BLOCK_SIZE) {
		printf("error: invalid start address\n");
		return 0;
	}

	dump_memory(out, G.block, (int) start, -1);
	return 1;
}

static int dump_2(FILE *out, ParsedCommand *pc) {
	unsigned int start;
	if (!hex_to_uint(pc->arguments[0], &start) || start >= BLOCK_SIZE) {
		printf("error: invalid start address\n");
		return 0;
	}

	unsigned int end;
	if (!hex_to_uint(pc->arguments[1], &end) || end >= BLOCK_SIZE || end < start) {
		printf("error: invalid end address\n");
		return 0;
	}

	dump_memory(out, G.block, (int) start, (int) end);
	return 1;
}

static int edit_2(FILE *out, ParsedCommand *pc) {
	unsigned int address;
	if (!hex_to_uint(pc->arguments[0], &address) || address >= BLOCK_SIZE) {
		fprintf(out, "error: invalid address\n");
		return 0;
	}
	unsigned int value;
	if (!hex_to_uint(pc->arguments[1], &value) || value >= 256) {
		fprintf(out, "error: invalid value\n");
		return 0;
	}
	set_memory(G.block, address, value);

	return 1;
}

static int fill_3(FILE *out, ParsedCommand *pc) {
	unsigned int start;
	if (!hex_to_uint(pc->arguments[0], &start) || start >= BLOCK_SIZE) {
		fprintf(out, "error: invalid start address\n");
		return 0;
	}
	unsigned int end;
	if (!hex_to_uint(pc->arguments[1], &end) || end >= BLOCK_SIZE || end < start) {
		fprintf(out, "error: invalid end address\n");
		return 0;
	}
	unsigned int value;
	if (!hex_to_uint(pc->arguments[2], &value) || value >= 256) {
		fprintf(out, "error: invalid value\n");
		return 0;
	}
	fill_memory(G.block, start, end ,value);

	return 1;
}

static int help_0(FILE *out, ParsedCommand *pc) {
	fprint_help(out);
	return 1;
}

static int reset_0(FILE *out, ParsedCommand *pc) {
	reset_memory(G.block);
	return 1;
}

static int opcode_1(FILE *out, ParsedCommand *pc) {
	unsigned char opcode;
	if (!find_from_hash_table(G.table, pc->arguments[0], &opcode)) {
		fprintf(out, "Couldn't find opcode for %s\n", pc->arguments[0]);
		return 0;
	}

	printf("opcode is %02X\n", opcode);
	return 1;
}

static int opcodelist_0(FILE *out, ParsedCommand *pc) {
	fprint_hash_table(out, G.table);
	return 1;
}

static int type_1(FILE *out, ParsedCommand *pc) {
	fprint_file(out, pc->arguments[0]);
	return 1;
}

static int bp_1(FILE *out, ParsedCommand *pc) {
	unsigned int value;
	if (!strcmp(pc->arguments[0], "clear")) {
		clear_breakpoints(G.block);
	} else {
		if (!hex_to_uint(pc->arguments[0], &value) || value >= BLOCK_SIZE) {
			fprintf(out, "error: invalid value\n");
			return 0;
		}
		set_breakpoint(stdout, G.block, value);
	}

	return 1;
}

static int bp_0(FILE *out, ParsedCommand *pc) {
	print_breakpoints(G.block);
	return 1;
}

static int progaddr_1(FILE *out, ParsedCommand *pc) {
	unsigned int value;
	if (!hex_to_uint(pc->arguments[0], &value) || value >= BLOCK_SIZE) {
		fprintf(out, "error: invalid value\n");
		return 0;
	}

	G.block->load_address = value;
	return 1;
}

static int run_0(FILE *out, ParsedCommand *pc) {
	run(G.block);
	return 1;
}
