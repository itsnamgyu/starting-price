#include "assemble.h"

#include <stdio.h>

static const int SIC_MNEMONIC_LENGTH = 10;
static const int SIC_LABEL_LENGTH = 7;

static const int ASM_LINE_TOO_LONG = 1;

typedef enum _OperandType {
	ADDRESS, REG, REG_PAIR, BYTE, WORD, RBYTES, RWORDS, NONE,
} OperandType;

typedef enum _AddressingMode {
	IMMEDIATE, INDIRECT, SIMPLE,
} AddressingMode;

typedef enum _Register {
	A = 0, X = 1, L = 2, B = 3, 
	S = 4, T = 5, PC = 8, SW = 9,
} Register;

typedef struct _SicStatementNode {
	unsigned int address;

	char label[SIC_LABEL_LENGTH];
	char is_ext;
	int base_value; // -1 for NOBASE
	char mnemonic[SIC_MNEMONIC_LENGTH];

	OperandType type;
	char *operand_string;
	union {
		struct { // memory operand
			AddressingMode mode;
			unsigned int memory;
		};
		struct { // single register
			Register r;
		};
		struct { // register pair
			Register r1;
			Register r2;
		};
		struct { // byte
			char *bytes;
		};
		struct { // word
			unsigned char word;
		}
		struct { // reserved size
			int rbytes
		};
	} operands;
} SicStatementNode;

typedef struct _SicTranslation {
	SicStatementNode head;
	SicStatementNode last;
} SicTranslation;

static SicCommandNode *read_line(FILE *in);

static char *read_line(FILE *in, int *error);
/*
 * Read one line of none-commented assembly from input stream
 * Return NULL on EOF
 *
 * Put results to *error:
 * 0 if successful
 * ASM_LINE_TOO_LONG if too long
 */

static int sscan_operand(char **asm_line_cursor, SicCommandNode *node);
/*
 * Scans an operand from asm line, according to the OperandType specified in
 * SicCommandNode. Save the results back to SicCommandNode.
 *
 * Ignore preceding whitespace, and move *asm_line_cursor to the leading
 * whitespace after the last character.
 *
 * Return 0 on error.
 */

static int sscan_label_and_reserved(char **asm_line_cursor, SicCommandNode *node);
/*
 * Scan for an optional label and reserved word (operator etc.) from asm line.
 * Save the results back to SicCommandNode.
 *
 * Ignore preceding whitespace, and move *string to the leading whitespace
 * after the last character.
 *
 * Return 0 on error.
 */

SicCommandNode *read_command(FILE *in) {
}

static char *read_line(FILE *in, int *error) {
	int is_start_of_line = 1;
	int is_comment_line = 0;
	char c;

	char buffer[4096] = { 0 };
	char *buffer_cursor = buffer;
	char *buffer_end = buffer + 1024;
	
	while ((c = fgetc(in)) != EOF) {
		if (is_comment_line) {
			if (c == '\n') {
				is_start_of_line = 1;
				is_comment_line = 0;
			}
			continue;
		}
		
		if (is_start_of_line && c == '.') {
			is_comment_line = 1;
			continue;
		}

		if (c == '\n') {
			char *string = malloc(sizeof(char) * strlen(buffer) + 1);
			strcpy(string, buffer);
			*error = 0;
			return string;
		}

		if (buffer_cursor == buffer_end) {
			*error = ASM_LINE_TOO_LONG;
			return NULL;
		}

		*(buffer_cursor++) = c;
	}

	if (is_start_of_line || is_comment_line) {
		*error = 0;
		return NULL;
	}
}
