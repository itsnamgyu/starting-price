#include "asm_helper.h"
#include "assemble.h"
#include "hashtable.h"
#include "generic_list.h"
#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define READ_LINE_INVALID -1
#define READ_LINE_SUCCESS 0
#define READ_LINE_EOF 1
#define READ_LINE_BLANK 2
static int read_line(FILE *in, char **string);
/*
 * Read one line from input stream and save to *string unless it is a comment
 * line. Save NULL to *string if it is a comment.
 *
 * Return Value
 * -1: too long
 * 0: successful
 * 1: eof
 */

static int sscan_operand(char **asm_line_cursor, SicStatement *statement);
/*
 * Scans an operand from asm line, according to the OperandType specified in
 * SicStatement. Save the results back to SicStatement.
 *
 * Ignore preceding whitespace, and move *asm_line_cursor to the leading
 * whitespace after the last character.
 *
 * Return 0 on error.
 */

static int sscan_label_and_reserved(
		char **asm_line_cursor, SicStatement *statement, 
		ReservedDict *reserved);
/*
 * Scan for an optional label and reserved word (operator etc.) from asm line.
 * Save the results back to SicStatement.
 *
 * Ignore preceding whitespace, and move *string to the leading whitespace
 * after the last character.
 *
 * Return 0 on error.
 */

static SicStatement *add_statement(SicStatementList *list);

#ifdef TEST
#undef TEST
#include "generic_list.c"
#include "generic_dict.c"
#include "symbol.c"
#include "reserved.c"
#include "hashtable.c"
int main(void) {
	return 0;
}
#endif

SicStatementList *new_statement_list() {
	return (SicStatementList*) new_list();
}

static void free_statement(void *s_void) {
	SicStatement *s = (SicStatement*) s_void;

	if (s->reserved_string) free(s->reserved_string);
	if (s->type == BYTE && !s->operands.is_b && s->operands.bytes)
		free(s->operands.bytes);
	if (s->type == BYTE && s->operands.is_b && s->operands.ubytes)
		free(s->operands.ubytes);
	free(s);
}

void free_statement_list(SicStatementList *list) {
	free_list(list, free_statement);
}

int read_asm(FILE *out, FILE *in, TranslationUnit *tu) {
	char *asm_line;
	int res;
	int line_number;
	for (line_number = 1; ; ++line_number) {
		res = read_line(in, &asm_line);
		if (asm_line) {
			char *cursor = asm_line;
			SicStatement *s = add_statement(tu->statements);
			s->line_number = line_number * 5;
			if (!sscan_label_and_reserved(&cursor, s, tu->reserved)) {
				fprintf(out, 
						"error: an error occured on line %d\n", line_number);
				fprintf(out, ">> %s\n", asm_line);
				return 0;
			}
			if (!sscan_operand(&cursor, s)) {
				fprintf(out, 
						"error: an error occured on line %d\n", line_number);
				fprintf(out, ">> %s\n", asm_line);
				return 0;
			}
			free(asm_line);
		} else {
			if (res == READ_LINE_EOF) return 1;
			if (res == READ_LINE_BLANK) continue;
			if (res == READ_LINE_INVALID) {
				fprintf(out, "error: could not read line number %d\n", line_number);
				return 0;
			}
		}
	}

	return 1;
}

int assign_addresses(FILE *out, TranslationUnit *tu) {
	int address = 0;
	int address_set = 0;
	for (LinkedNode *node = tu->statements->head->link;
			node; node = node->link) {
		SicStatement *s = (SicStatement*) node->value;
		if (s->type == START) {
			address = s->operands.start_address;
			address_set = 1;
			break;
		}
	}

	if (!address_set) {
		fprintf(out, "error: there is no START directive\n"); return 0;
	}

	for (LinkedNode *node = tu->statements->head->link;
			node; node = node->link) {
		SicStatement *s = (SicStatement*) node->value;
		s->address = address;
		switch (s->type) {
			case START:
			case END:
			case BASE:
			case NOBASE:
				s->instruction_size = 0; break;
			case BYTE:
				s->instruction_size = strlen(s->operands.bytes); break;
			case WORD:
				s->instruction_size = 3; break;
			case RBYTE:
			case RWORD:
				s->instruction_size = s->operands.rsize; break;
			case REG_OPERATION:
			case REG_PAIR_OPERATION:
				s->instruction_size = 2; break;
			case MEM_OPERATION:
				if (s->e) s->instruction_size = 4;
				else s->instruction_size = 3;
				break;
			case BLANK_OPERATION:
				s->instruction_size = 3; break;
			default:
				fprintf(out, "error: operator unsupported\n");
				return 0;
		}
		address += s->instruction_size;
	}
	
	return 1;
}

int fill_symbol_table(FILE *out, TranslationUnit *tu) {
	for (LinkedNode *node = tu->statements->head->link;
			node; node = node->link) {
		SicStatement *s = (SicStatement*) node->value;
		if (*s->label) {
			if (dict_contains(tu->symbols, s->label)) {
				fprintf(out, "error: duplicate symbol on line %d\n", s->line_number);
				return 0;
			} else {
				add_to_symbol_table(tu->symbols, s->label, s->address);
			}
		}
	}

	return 1;
}

int validate_statements(FILE *out, TranslationUnit *tu) {
	int base = -1;
	for (LinkedNode *node = tu->statements->head->link;
			node; node = node->link) {
		SicStatement *s = (SicStatement*) node->value;
		unsigned int absolute_address;
		s->base = base;
		switch (s->type) {
			case START: case END: case BYTE: case WORD: case RBYTE:
			case RWORD: case REG_OPERATION: case REG_PAIR_OPERATION:
			case BLANK_OPERATION:
				break;
			case BASE:
				if (!find_symbol_address(
							tu->symbols, s->operands.label, &absolute_address)) {
					fprintf(out, "error: undefined symbol on line %d\n", s->line_number);
					return 0;
				}
				base = absolute_address;
				break;
			case NOBASE:
				base = -1;
				break;
			case MEM_OPERATION:
				if (s->operands.is_literal) {
					absolute_address = s->operands.literal;
					if (!s->e) {
						if (absolute_address > 4096) {
							fprintf(out, "error: requires extended instruction on line %d\n",
									s->line_number);
							return 0;
						} 
					}
					s->operands.adjusted_address = absolute_address;
				} else {
					if (!find_symbol_address(
								tu->symbols, s->operands.label, &absolute_address)) {
						fprintf(out, "error: undefined symbol on line %d\n", s->line_number);
						return 0;
					}

					if (s->e) s->operands.adjusted_address = absolute_address;
					else {
						int pc = absolute_address -
							(s->address + s->instruction_size);
						int b = absolute_address - s->base;
						if (-2048 <= pc && pc <= 2047) {
							s->p = 1; s->b = 0;
							s->operands.adjusted_address = pc;
						} else if (s->base != -1 && 0 <= b && b <= 4096) {
							s->p = 0; s->b = 1;
							s->operands.adjusted_address = b;
						} else {
							fprintf(out, "error: requires extended instruction on line %d\n",
									s->line_number);
							return 0;
						}
					}
				}

				break;
			default:
				return 0;
		}
	}
	return 1;
}

static inline int is_whitespace(char c) {
	return c == ' ' || c == '\t';
}

static inline int is_whitespace_or_comma(char c) {
	return c == ' ' || c == '\t' || c == ',';
}

static int read_line(FILE *in, char **string) {
	int is_start_of_line = 1;
	int is_comment_part = 0;
	int c;

	char buffer[4096] = { 0 };
	char *buffer_cursor = buffer;
	char *buffer_end = buffer + 1024;
	
	if (feof(in)) {
		*string = NULL;
		return READ_LINE_EOF;
	}

	while (1) {
		c = fgetc(in);
		if (c == '\n' || c == EOF) {
			if (is_start_of_line) {
				*string = NULL;
				return READ_LINE_BLANK;
			} else {
				*string = malloc(sizeof(char) * (strlen(buffer) + 1));
				strcpy(*string, buffer);
				return READ_LINE_SUCCESS;
			}
		}
		if (is_comment_part) {
			continue;
		} else {
			if (c == '.') {
				is_comment_part = 1; continue;
			}

			if (is_start_of_line && !is_whitespace(c)) is_start_of_line = 0;

			if (!is_start_of_line) {
				if (buffer_cursor == buffer_end) return READ_LINE_INVALID;
				*buffer_cursor++ = c;
			}
		}
	}
}

static inline void skip_whitespaces(char **cursor) {
	while (is_whitespace(**cursor) && **cursor) {
		(*cursor) += 1;
	}
}

static inline int skip_whitespaces_and_comma(char **cursor) {
	int passed_comma = 0;
	while (**cursor && is_whitespace_or_comma(**cursor)) {
		if (**cursor == ',') {
			if (passed_comma) return 0;
			else passed_comma = 1;
		}
		(*cursor) += 1;
	}

	return passed_comma;
}

#define SSCAN_WORD_BUFFER_SIZE 33
#define SSCAN_WORD_NO_COMMA 0
#define SSCAN_WORD_REQUIRE_COMMA 1
static char *sscan_word(char **cursor, int require_comma_separator) {
	char buffer[SSCAN_WORD_BUFFER_SIZE];
	char *buffer_cursor = buffer;
	char *buffer_end = buffer + SSCAN_WORD_BUFFER_SIZE;
	
	if (require_comma_separator) {
		if (!skip_whitespaces_and_comma(cursor))
			return 0;
	} else skip_whitespaces(cursor);
	
	for (; !is_whitespace_or_comma(**cursor) && **cursor; (*cursor)++, buffer_cursor++) {
		if (buffer_cursor == buffer_end - 1)
			return NULL;
		*buffer_cursor = **cursor;
	}
	*buffer_cursor = '\0';

	if (buffer_cursor == buffer) return NULL;

	char *string = malloc(sizeof(char) * (buffer_cursor - buffer + 1));
	strcpy(string, buffer);
	
	return string;
}

#define SSCAN_NO_COMMA 0
#define SSCAN_REQUIRE_COMMA 1
static int sscan_int(char **cursor, int require_comma_separator) {
	int n, offset;

	if (require_comma_separator) {
		if (!skip_whitespaces_and_comma(cursor))
			return -1;
	} else skip_whitespaces(cursor);
	
	if (sscanf(*cursor, "%d%n", &n, &offset) != 1) {
		return -1;
	}
	(*cursor) += offset;

	if (is_whitespace_or_comma(**cursor)) return -1; // trailing chars

	return n;
}

static char *register_to_string(Register reg) {
	char *strings[8] = {
		"A", "X", "L", "B", "S", "T", "PC", "SW" };
	Register registers[8] = {
		A, X, L, B, S, T, PC, SW };
	
	for (int i = 0; i < 8; ++i) {
		if (registers[i] == reg) {
			return strings[i];
		}
	}

	return NULL;
}

static int sscanf_register(
		char **cursor, Register *reg, int require_comma_separator) {
	char *word = sscan_word(cursor, require_comma_separator);
	if (!word) return 0;
	
	char *strings[8] = {
		"A", "X", "L", "B", "S", "T", "PC", "SW" };
	Register registers[8] = {
		A, X, L, B, S, T, PC, SW };


	for (int i = 0; i < 8; ++i) {
		if (!strcmp(strings[i], word)) {
			*reg = registers[i];
			free(word);
			return 1;
		}
	}

	free(word);
	return 0;
}

static int read_memory_operand(char *word, SicStatement *statement) {
	switch (*word) {
		case '@': statement->operands.mode = INDIRECT; word++; break;
		case '#': statement->operands.mode = IMMEDIATE; word++; break;
		default: statement->operands.mode = SIMPLE;
	}

	int literal = -1;
	if (statement->operands.mode == IMMEDIATE)
		// may be literal if immediate
		literal = sscan_int(&word, SSCAN_NO_COMMA);

	if (literal == -1) {
		if (strlen(word) >= SIC_LABEL_LENGTH) return 0;
		strcpy(statement->operands.label, word);
		statement->operands.is_literal = 0;
	} else {
		statement->operands.literal = literal;
		statement->operands.is_literal = 1;
	}

	return 1;
}

static inline int isupperxdigit(char c) {
	return toupper(c) == c && isxdigit(c);
}

static int read_bytes(char *word, SicStatement *statement) {
	int is_bytes; // or chars
	switch (*(word++)) {
		case 'X': is_bytes = 1; break;
		case 'C': is_bytes = 0; break;
		default: return 0;
	}
	if (*(word++) != '\'') return 0;


	char bytes[SIC_MAX_BYTE_LENGTH] = { 0 };
	unsigned char ubytes[SIC_MAX_BYTE_LENGTH] = { 0 };
	int index = 0;
	statement->operands.is_b = is_bytes;
	if (is_bytes) {
		int odd = 1;
		while (isupperxdigit(*word)) {
			if (index == SIC_MAX_BYTE_LENGTH - 1) return 0;
			int n; sscanf(word++, "%1X", &n);
			
			if (odd) ubytes[index] += 16 * n;
			else ubytes[index++] += n;

			odd = !odd;
		}
		// if odd == TRUE, the number of hexes is even
		if (!odd) index++;
		statement->operands.is_odd_b = !odd;
	} else {
		while (*word != '\'') {
			if (!*word) return 0;
			if (index == SIC_MAX_BYTE_LENGTH - 1) return 0;
			bytes[index++] = *(word++);
		}
		bytes[index] = '\0';
	}

	if (*(word++) != '\'') return 0;
	if (*word != '\0') return 0;

	if (is_bytes) {
		statement->operands.ubytes = malloc(sizeof(unsigned char));
		statement->operands.ulength = index;
		for (int i = 0; i < index; ++i)
			statement->operands.ubytes[i] = ubytes[i];
	} else {
		statement->operands.bytes = malloc(sizeof(char) * (strlen(bytes) + 1));
		strcpy(statement->operands.bytes, bytes);
	}

	return 1;
}

static int sscan_operand(char **asm_line_cursor, SicStatement *statement) {
	// In all asm lines, the reserved token is preceded by an appropriate
	// operand for the ReservedType within statement
	char *first_word;
	char *second_word;
	int n;
	switch (statement->type) {
		case MEM_OPERATION:
			first_word = sscan_word(asm_line_cursor, SSCAN_WORD_NO_COMMA);
			if (!first_word) return 0;
			if (!read_memory_operand(first_word, statement)) {
				free(first_word); return 0;
			}

			// no ,X
			skip_whitespaces(asm_line_cursor);
			if (**asm_line_cursor == '\0') {
				statement->operands.x = 0; break;
			}
			
			// must have , X
			second_word = sscan_word(asm_line_cursor, SSCAN_WORD_REQUIRE_COMMA);
			if (!second_word) return 0;
			if (strcmp(second_word, "X")) {
				free(second_word);
				return 0;
			}
			statement->operands.x = 1;
			free(second_word);
			break;

		case REG_OPERATION:
			if (!sscanf_register(asm_line_cursor, 
						&statement->operands.r, SSCAN_NO_COMMA))
				return 0;
			break;

		case REG_PAIR_OPERATION:
			if (!sscanf_register(asm_line_cursor, 
						&statement->operands.r1, SSCAN_NO_COMMA))
				return 0;
			if (!sscanf_register(asm_line_cursor, 
						&statement->operands.r2, SSCAN_REQUIRE_COMMA))
				return 0;
			break;

		case BLANK_OPERATION: break;

		case BYTE:
			first_word = sscan_word(asm_line_cursor, SSCAN_WORD_NO_COMMA);
			if (!first_word) return 0;
			if (!read_bytes(first_word, statement)) {
				free(first_word); return 0;
			}
			free(first_word);
			break;

		case WORD:
			if ((n = sscan_int(asm_line_cursor, SSCAN_NO_COMMA)) == -1)
				return 0;
			statement->operands.word = n;
			break;

		case RBYTE:
			if ((n = sscan_int(asm_line_cursor, SSCAN_NO_COMMA)) == -1)
				return 0;
			statement->operands.rsize = n;
			break;

		case RWORD:
			if ((n = sscan_int(asm_line_cursor, SSCAN_NO_COMMA)) == -1)
				return 0;
			statement->operands.rsize = n * 3;
			break;

		case START:
			if ((n = sscan_int(asm_line_cursor, SSCAN_NO_COMMA)) == -1) {
				return 0;
			}
			statement->operands.start_address = n;
			break;

		case END:
			first_word = sscan_word(asm_line_cursor, SSCAN_WORD_NO_COMMA);
			if (first_word) {
				if (strlen(first_word) >= SIC_LABEL_LENGTH) return 0;
				strcpy(statement->operands.label, first_word);
				free(first_word);
			} else statement->operands.label[0] = '\0';
			break;

		case BASE:
			first_word = sscan_word(asm_line_cursor, SSCAN_WORD_NO_COMMA);
			if (strlen(first_word) >= SIC_LABEL_LENGTH) return 0;
			strcpy(statement->operands.label, first_word);
			free(first_word);
			break;

		case NOBASE: break;
					 
		default:
			printf("Unsupported operand\n");
			return 0;
	}
	// check for trailing text
	skip_whitespaces(asm_line_cursor);
	return **asm_line_cursor == '\0';
}

static inline int is_reserved(char *word, ReservedDict *reserved) {
	if (*word == '+') return dict_contains(reserved, word + 1);
	else return dict_contains(reserved, word);
}

static int read_reserved(
		char *word, SicStatement *statement, ReservedDict *reserved) {
	if (*word == '+') {
		// this instruction is extended so this must be a memory operator
		if (find_reserved_type(reserved, word + 1, &statement->type)) {
			if (statement->type == MEM_OPERATION) {
				statement->reserved_string = malloc(
						sizeof(char) * (strlen(word) - 1));
				strcpy(statement->reserved_string, word + 1);
				statement->e = 1;
				free(word);
				return 1;
			}
		}
	}
	else {
		if (find_reserved_type(reserved, word, &statement->type)) {
			statement->reserved_string = word;
			statement->e = 0;
			return 1;
		}
	}
	return 0;
}

static int sscan_label_and_reserved(
		char **asm_line_cursor, SicStatement *statement, 
		ReservedDict *reserved) {
	// All asm lines start with an optional label and a required
	// reserved token like an operand or START, RBYTE etc.
	char *first_word = sscan_word(asm_line_cursor, SSCAN_WORD_NO_COMMA);
	if (!first_word) return 0;

	if (is_reserved(first_word, reserved)) {
		// we found a reserved token, so there is no label
		return read_reserved(first_word, statement, reserved);
	}

	// else -> save the label
	if (strlen(first_word) >= SIC_LABEL_LENGTH) return 0;

	strcpy(statement->label, first_word);
	free(first_word);

	char *second_word = sscan_word(asm_line_cursor, SSCAN_WORD_NO_COMMA);
	if (!second_word) return 0;

	// now we need to find a reserved token
	if (is_reserved(second_word, reserved)) {
		return read_reserved(second_word, statement, reserved);
	}
	free(second_word);
	return 0;
}

static SicStatement *add_statement(SicStatementList *list) {
	SicStatement *statement = calloc(sizeof(SicStatement), 1);
	statement->reserved_string = NULL;
	statement->operands.bytes = NULL;
	statement->p = 0;
	statement->b = 0;
	statement->e = 0;
	add_to_list(list, (void*) statement);

	return statement;
}

static void strcat_byte(char *string, SicStatement *statement, int all_hex) {
	if (statement->operands.is_b) {
		char hex[3];
		for (int i = 0; i < statement->operands.ulength; ++i) {
			if (statement->operands.is_odd_b &&
					i == statement->operands.ulength - 1) {
				sprintf(hex, "%1X", statement->operands.ubytes[i] / 16);
			} else {
				sprintf(hex, "%02X", statement->operands.ubytes[i]);
			}
			strcat(string, hex);
		}
	} else {
		if (all_hex) {
			for (char *c = statement->operands.bytes; *c; ++c) {
				char hex[3];
				sprintf(hex, "%02X", *c);
				strcat(string, hex);
			}
		} else {
			strcat(string, statement->operands.bytes);
		}
	}
}

void fprint_lst_line(FILE *out, SicStatement *statement, HashTable *opcodes) {
	fprintf(out, "%3d", statement->line_number);

	if (statement->type == BASE || statement->type == NOBASE)
		fprintf(out, "   %4s", "");
	else
		fprintf(out, "   %04X", statement->address);

	fprintf(out, "   %-6s", *statement->label ? statement->label : "");
	
	fprintf(out, "  ");
	if (statement->e) fprintf(out, "+");
	else fprintf(out, " ");
	fprintf(out, "%-9s", statement->reserved_string);

	char key = ' ';
	if (statement->type == MEM_OPERATION) {
		if (statement->operands.mode == INDIRECT) key = '@';
		else if (statement->operands.mode == IMMEDIATE) key = '#';
	}
	fprintf(out, "  ");
	fprintf(out, "%c", key);
	int length;
	char *operand;
	switch (statement->type) {
		case MEM_OPERATION:
			if (statement->operands.is_literal) {
				fprintf(out, "%-12d", statement->operands.literal);
			} else {
				length = strlen(statement->operands.label);
				operand = malloc(sizeof(char) * (length + 3));

				strcpy(operand, statement->operands.label);
				if (statement->operands.x) strcat(operand, ",X");
				fprintf(out, "%-12s", operand);
				free(operand);
			}
			break;

		case REG_OPERATION:
			fprintf(out, "%-12s", register_to_string(statement->operands.r));
			break;

		case REG_PAIR_OPERATION:
			operand = malloc(sizeof(char) * 10);
			strcpy(operand, register_to_string(statement->operands.r1));
			strcat(operand, ",");
			strcat(operand, register_to_string(statement->operands.r2));

			fprintf(out, "%-12s", operand);
			free(operand);
			break;

		case BYTE:
			if (statement->operands.is_b) {
				length = statement->operands.ulength;
				operand = malloc(sizeof(unsigned char) * (length * 2 + 4));
				strcpy(operand, "X'");
				strcat_byte(operand, statement, 0);
				strcat(operand, "'");
			} else {
				length = strlen(statement->operands.bytes);
				operand = malloc(sizeof(char) * (length * 2 + 4));
				strcpy(operand, "C'");
				strcat_byte(operand, statement, 0);
				strcat(operand, "'");
			}
			fprintf(out, "%-12s", operand);
			free(operand);
			break;
			
		case WORD:
			fprintf(out, "%-12d", statement->operands.word);
			break;

		case RBYTE:
			fprintf(out, "%-12d", statement->operands.rsize);
			break;

		case RWORD:
			fprintf(out, "%-12d", statement->operands.rsize / 3);
			break;

		case START:
			fprintf(out, "%-12d", statement->operands.start_address);
			break;

		case END:
			fprintf(out, "%-12s", statement->operands.label);
			break;

		case BASE: case NOBASE: case BLANK_OPERATION:
			fprintf(out, "%12s", "");
			break;

		case REG_N_OPERATION: case N_OPERATION:
			assert(0 && "unsupported operator");
			break;
	}

	fprint_statement_in_hex(out, statement, opcodes);
}

void fprint_statement_in_hex(FILE *out, SicStatement *statement, 
		HashTable *opcodes) {
	unsigned char opcode;	
	if (statement->type & RESERVED_OPERATION_FLAG) {
		if (!find_from_hash_table(opcodes, statement->reserved_string, 
					&opcode)) {
			assert(0 && "couldn't find operation");
		}
	}
	
	char buffer[SIC_MAX_BYTE_LENGTH] = { 0 };
	switch (statement->type) {
		case MEM_OPERATION:
			switch (statement->operands.mode) {
				case IMMEDIATE: opcode = opcode | 1; break;
				case INDIRECT: opcode = opcode | 2; break;
				case SIMPLE: opcode = opcode | 3; break;
			}
			fprintf(out, "%02X", opcode);

			int address = statement->operands.adjusted_address;
			if (statement->e) {
				address = address | (1 << 20); // set e flag
				// set x flog
				if (statement->operands.x) address = address | (1 << 23);
				fprintf(out, "%06X", address);
			} else {
				if (statement->p) {
					int signed_bit = address < 0;
					if (address < 0) {
						address = address & ((1 << 11) - 1);
					}
					address = address | (signed_bit << 11);
					address = address | (1 << 13); // set p flag
				} else if (statement->b) {
					address = address | (1 << 14); // set b flag
				}
				// set x flog
				if (statement->operands.x) address = address | (1 << 15);
				fprintf(out, "%04X", address);
			}
			break;

		case REG_OPERATION:
			fprintf(out, "%02X", opcode);
			fprintf(out, "%1X", statement->operands.r);
			fprintf(out, "0");
			break;

		case REG_PAIR_OPERATION:
			fprintf(out, "%02X", opcode);
			fprintf(out, "%1X", statement->operands.r1);
			fprintf(out, "%1X", statement->operands.r2);
			break;

		case BLANK_OPERATION:
			opcode = opcode | 3; // SIMPLE mode by default
			fprintf(out, "%02X", opcode);
			fprintf(out, "0000");
			break;

		case REG_N_OPERATION: case N_OPERATION:
			assert(0 && "unsupported operator");
			break;

		case BYTE:
			strcat_byte(buffer, statement, 1);
			fprintf(out, "%s", buffer);
			break;

		case WORD:
			fprintf(out, "%04X", statement->operands.word);
			break;

		default:
			break;
	}
}

void fprint_header_record(FILE *out, TranslationUnit *tu) {
	fprintf(out, "H");

	int last_address = 0;
	for (LinkedNode *node = tu->statements->head->link; node; 
			node = node->link) {
		SicStatement *statement = (SicStatement*) node->value;
		if (statement->type == START) {
			fprintf(out, "%-6s", statement->label);
			fprintf(out, "%06X", statement->operands.start_address);
		}
		last_address = statement->address;
	}
	fprintf(out, "%06X", last_address);
	fprintf(out, "\n");
}

static void fflush_text_record_line(FILE *out, HashTable *opcodes,
		LinkedNode *start_node, LinkedNode *end_node) {
	for (LinkedNode *node = start_node; node != end_node; node = node->link)
		fprint_statement_in_hex(out, (SicStatement*) node->value, opcodes);
}

#define TEXT_RECORD_SIZE 32 // 20
void fprint_text_record(FILE *out, TranslationUnit *tu) {
	LinkedNode *line_start_node;
	int address = 0;
	int line_start_address = 0;
	int length = 0;

	line_start_node = tu->statements->head->link;
	for (LinkedNode *node = line_start_node; node; node = node->link) {
		SicStatement *s = (SicStatement*) node->value;
		int size = s->instruction_size;
		if (size) {
			int is_reserved = (s->type == RWORD || s->type == RBYTE);
			int newline = length != 0 && length + size >= TEXT_RECORD_SIZE;

			if ((is_reserved && length) || newline ) {
				fprintf(out, "T");
				fprintf(out, "%06X", line_start_address);
				fprintf(out, "%02X", length);
				fflush_text_record_line(
						out, tu->opcodes, line_start_node, node);
				fprintf(out, "\n");

				line_start_address = address;
				line_start_node = node;
				length = 0;
			}

			if (!is_reserved) length += size;
			address += size;
		}
	}

	if (length) {
		fprintf(out, "T");
		fprintf(out, "%06X", line_start_address);
		fprintf(out, "%02X", length);
		fflush_text_record_line(out, tu->opcodes, line_start_node, NULL);
		fprintf(out, "\n");
	}
}

void fprint_end_record(FILE *out, TranslationUnit *tu) {
	fprintf(out, "E");
	for (LinkedNode *node = tu->statements->head->link; node; 
			node = node->link) {
		SicStatement *statement = (SicStatement*) node->value;
		if (statement->type == END) {
			unsigned int exec_address;
			find_symbol_address(tu->symbols, 
					statement->operands.label, &exec_address);
			fprintf(out, "%06X", exec_address);
			fprintf(out, "\n");
			return;
		}
	}
}
