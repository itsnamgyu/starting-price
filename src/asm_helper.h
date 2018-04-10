/*
 * asm_helper.h
 *
 * This module helps assemble.h with the extrenuous assembly process of asm
 * files. Why? Because there are just too many functions and too much code.
 *
 */

#ifndef ASM_HELPER_H
#define ASM_HELPER_H
#include "reserved.h"
#include "symbol.h"
#include "hashtable.h"
#include "generic_list.h"
#include "register.h"

#include <stdio.h>

#define SIC_MNEMONIC_LENGTH 10
#define SIC_LABEL_LENGTH 7
#define SIC_MAX_BYTE_LENGTH 4096

typedef enum _AddressingMode {
	IMMEDIATE, INDIRECT, SIMPLE,
} AddressingMode;

typedef struct _SicStatement {
	unsigned int address;
	unsigned int instruction_size;
	int line_number;

	char label[SIC_LABEL_LENGTH];
	int base; // -1 for NOBASE

	char p, b, e;

	ReservedType type;
	char *reserved_string;
	union {
		struct { // memory operand or end
			AddressingMode mode;
			char x;
			char is_literal;
			int adjusted_address;
			struct {
				char label[SIC_LABEL_LENGTH];
				unsigned int literal;
			};
		};
		struct { // single register
			Register r;
		};
		struct { // register pair
			Register r1;
			Register r2;
		};
		struct { // byte
			char is_b; // TRUE: X'0E1', FALSE: C'HELLO'
			char is_odd_b; // TRUE: X'203', FALSE: X'2034'
			int ulength; // The number of hexes in X'*'
			union {
				char *bytes; // The string within C'*'
				unsigned char *ubytes; // The array of bytes within X'*'
			};
		};
		struct { // word
			unsigned int word;
		};
		struct { // reserved size
			int rsize;
		};
		struct {
			int start_address;
		};
	} operands;
} SicStatement;

#define SicStatementList List

typedef struct _TranslationUnit {
	HashTable *opcodes;
	ReservedDict *reserved;
	SicStatementList *statements;
	SymbolTable *symbols;
} TranslationUnit;

SicStatementList *new_statement_list();

void free_statement_list(SicStatementList *list);

int read_asm(FILE *out, FILE *in, TranslationUnit *tu);
/*
 * Read all SIC/XE statements from input stream. = Pass 1
 * 
 * This reads a non-comment line from asm source code and parses labels, 
 * reserved words (operators or directives) and optionally operands and
 * n/i/e flags. The parsed results are stored in the SicStatementNode.
 * Also validate operator-operand compatability and determines the size
 * of the instruction. Also assigns line number of SicStatements
 * 
 * - Does count START, BASE, NOBASE directives as SicStatement's.
 * - Does not validate symbols, address-offsets etc.
 *
 * Print errors to output stream
 *
 * Returns success as boolean
 */

int assign_addresses(FILE *out, TranslationUnit *tu);

int fill_symbol_table(FILE *out, TranslationUnit *tu);
/*
 * Reads the sic statements fills the symbol table (all within tu)
 *
 * Print errors to output stream
 *
 * Returns success as boolean
 */

int validate_statements(FILE *out, TranslationUnit *tu);
/*
 * Validates the statements in tu. This assigns the p, b, e flag and checks
 * if p or b relative offsets can be used for non-extended instructions. Also
 * assigns the offset or absolute address that'll be used in the actual
 * instruction. Also sets the base addresses.
 *
 * Print errors to output stream
 *
 * Returns success as boolean
 */

void fprint_lst_line(FILE *out, SicStatement *statement, HashTable *opcodes);
/*
 * Prints the validated statement as a line in an lst file.
 */

void fprint_statement_in_hex(FILE *out, SicStatement *statement, 
		HashTable *opcodes);
/*
 * Prints the hex representation of the instruction stated by the
 * validated SicStatement.
 */

void fprint_header_record(FILE *out, TranslationUnit *tu);
/*
 * Prints the header record for translation unit tu.
 */

void fprint_text_record(FILE *out, TranslationUnit *tu);
/*
 * Prints the text record for translation unit tu.
 */

void fprint_end_record(FILE *out, TranslationUnit *tu);
/*
 * Prints the end record for translation unit tu.
 */

void fprint_modificaiton_record(FILE *out, TranslationUnit *tu);
/*
 * Prints the modification record for translation unit tu.
 */
#endif
