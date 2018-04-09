#include <stdio.h>
#include <stdlib.h>

#include "assemble.h"
#include "hashtable.h"
#include "reserved.h"
#include "symbol.h"
#include "asm_helper.h"
#include "global.h"
#include "string.h"

/* Recall these definitions
typedef struct _TranslationUnit {
	HashTable *opcodes;
	ReservedDict *reserved;
	SicStatementList *statements;
	SymbolTable *symbols;
} TranslationUnit;
*/

static TranslationUnit *tu = NULL;

int assemble_1(FILE *out, ParsedCommand *pc) {
	if (tu) {
		free_translation_unit(tu);
		tu = NULL;
	}

	FILE *in;
	if (!(in = fopen(pc->arguments[0], "r"))) {
		fprintf(out, "error: could not open file for assembly\n");
		return 0;
	}

	tu = translate(out, in, G.table, G.reserved);
	fclose(in);

	char lst[1000];
	char obj[1000];
	strcpy(lst, pc->arguments[0]);
	strcpy(obj, pc->arguments[0]);
	strcpy(lst + strlen(pc->arguments[0]) - 4, ".lst");
	strcpy(obj + strlen(pc->arguments[0]) - 4, ".obj");

	FILE *lst_out = fopen(lst, "w");
	FILE *obj_out = fopen(obj, "w");
	generate_lst(lst_out, tu);
	generate_obj(obj_out, tu);
	fclose(lst_out);
	fclose(obj_out);

	fprintf(out, "output file : [%s], [%s]\n", lst, obj);

	return 1;
}

int symbol_0(FILE *out, ParsedCommand *pc) {
	if (tu)
		fprint_symbols(out, tu->symbols);
	else
		fprintf(out, "there is no valid symbol table to print out.\n");
	return 1;
}

static TranslationUnit *new_translation_unit(
		FILE *in, HashTable *opcodes, ReservedDict *reserved) {
	TranslationUnit *tu = malloc(sizeof(TranslationUnit));
	
	tu->opcodes = opcodes;
	tu->reserved = reserved;
	tu->statements = new_statement_list();
	tu->symbols = new_symbol_table();

	return tu;
}

TranslationUnit *translate(
		FILE *out, FILE *in, HashTable *opcodes, ReservedDict *reserved) {
	TranslationUnit *tu = new_translation_unit(in, opcodes, reserved);

	if (!read_asm(out, in, tu)) return NULL;
	if (!assign_addresses(out, tu)) return NULL;
	if (!fill_symbol_table(out, tu)) return NULL;
	if (!validate_statements(out, tu)) return NULL;

	return tu;
}

void free_translation_unit(TranslationUnit *tu) {
	free_statement_list(tu->statements);
	free_symbol_table(tu->symbols);
	free(tu);
}

void generate_obj(FILE *out, TranslationUnit *tu) {
	fprint_header_record(out, tu);
	fprint_text_record(out, tu);
	fprint_end_record(out, tu);
}

void generate_lst(FILE *out, TranslationUnit *tu) {
	for (LinkedNode *node = tu->statements->head->link; node; 
			node = node->link) {
		fprint_lst_line(out, (SicStatement*) node->value, tu->opcodes);
		fprintf(out, "\n");
	}
}
