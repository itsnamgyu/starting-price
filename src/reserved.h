/*
 * reserved.h
 *
 * To parse asm files, we need a dictionary of reserved words, including
 * operators and directives like START, END, BASE etc. This module provides
 * the ReservedDict type that you can use to read a record file of reserved
 * words and interpret them during asm assembly.
 *
 * The functions below are basically wrapper fuctions for the generic_dict
 * module, so they are pretty self-explanatory.
 */

#ifndef RESERVED_H
#define RESERVED_H
#include "generic_dict.h"

#define RESERVED_OPERATION_FLAG 8
typedef enum _ReservedType {
	MEM_OPERATION = 8,
	REG_OPERATION = 9,
	REG_PAIR_OPERATION = 10,
	REG_N_OPERATION = 11,
	N_OPERATION = 12,
	BLANK_OPERATION = 13,
	BYTE = 0,
	WORD = 1,
	RBYTE = 2,
	RWORD = 3,
	START = 4,
	END = 5,
	BASE = 6,
	NOBASE = 7
} ReservedType;

typedef struct _ReservedValue {
	ReservedType type;
} ReservedValue;

typedef Dict ReservedDict;

ReservedDict *new_reserved_dict();

void free_reserved_dict(ReservedDict *dict);

void add_to_reserved_dict(ReservedDict *dict, char *key, ReservedType type);

int find_reserved_type(ReservedDict *dict, char *key, ReservedType *type);
#endif
