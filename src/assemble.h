#ifndef ASSEMBLE_H
#define ASSEMBLE_H
#include "hashtable.h"
#include "reserved.h"
#include "symbol.h"
#include "asm_helper.h"
#include "parser.h"

int assemble_1(FILE *out, ParsedCommand *pc);

int symbol_0(FILE *out, ParsedCommand *pc);

TranslationUnit *translate(
		FILE *out, FILE *in, HashTable *opcodes, ReservedDict *reserved);

void free_translation_unit(TranslationUnit *tu);

void generate_obj(FILE *out, TranslationUnit *tu);

void generate_lst(FILE *out, TranslationUnit *tu);

void print_symlist(FILE *out, TranslationUnit *tu);
#endif
