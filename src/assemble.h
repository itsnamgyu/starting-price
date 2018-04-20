#pragma once
/*
 * assemble.h
 *
 * This is where the assembly happens.
 * 
 * In a nutshell, this is our assembly process:
 * Read the assembly file, process it and save the translated SIC/XE code
 * as a TranslationUnit. Invidiual SIC/XE statements are saved within a
 * TranslationUnit as a linked-list of SicStatement's - which contain all
 * the information needed to generate the .lst and .obj files.
 *
 * Note that most of the implementations involve calling helper functions from
 * asm_helper.h.
 */
#include "hashtable.h"
#include "reserved.h"
#include "symbol.h"
#include "asm_helper.h"
#include "parser.h"

int assemble_1(FILE *out, ParsedCommand *pc);
/*
 * Assembles pc->argument[0] (*.asm) and generates an obj file and an lst
 * file.
 *
 * Return success as boolean.
 */

int symbol_0(FILE *out, ParsedCommand *pc);
/*
 * Prints out the symbol table of the previously assembled SIC/XE program.
 *
 * Return TRUE (since there's no way to fail..?)
 */

TranslationUnit *translate(
		FILE *out, FILE *in, HashTable *opcodes, ReservedDict *reserved);
/*
 * Translate the given asm file stream *in* into a TranslationUnit.
 *
 * Return the resulting TranslationUnit on success.
 * Return NULL on fail.
 */

void free_translation_unit(TranslationUnit *tu);
/*
 * Deallocates the translation unit
 */

void generate_obj(FILE *out, TranslationUnit *tu);
/*
 * Generates object code from the TranslationUnit and print it to the *out*
 * stream.
 */

void generate_lst(FILE *out, TranslationUnit *tu);
/*
 * Generates a listing record from the TranslationUnit and print it to the
 * *out* stream.
 */

void print_symlist(FILE *out, TranslationUnit *tu);
/*
 * Print the list of symbols defined in the TranslationUnit and print it to
 * the *out* stream.
 */
