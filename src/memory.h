#pragma once
#include <stdio.h>
#include <stdbool.h>

#include "generic_list.h"

#define BLOCK_SIZE 1048576
#define BLOCK_BUFFER_SIZE 32
#define MAX_DUMP_LENGTH 1000000

typedef struct _Block {
	unsigned char _prebuffer[32];
	unsigned char data[BLOCK_SIZE];
	unsigned char _postbuffer[32];
	int current;
	unsigned int load_address;
	unsigned int registers[15];
	List *breakpoints;
	unsigned int start_address;
} Block;
/*	Description
 *	A memory block that stores 1 megabyte of unsigned chars.
 *
 *	Members
 *	unsigned char data[]: 
 *	int current: the current position used for dump operations
 */


Block *new_memory_block();
/*	Description
 *	Initiate memory for a valid memory block.
 */

void set_memory(Block *block, int location, unsigned char value);
/*	Description
 *  Find the value associated with key from table.
 *
 *  Arguments
 *  char *key: key as string
 *  Value *value: return found value to reference
 *
 *  Return
 *  A boolean value for whether the key exists
 */

void fill_memory(Block *block, int start, int end, unsigned char value);
/*	Description
 *  Fill the memory block with a given value from start to end (inclusive).
 */

void dump_memory(FILE *out, Block *block, int start, int end);
/*	Description
 *  Print the values within the block from start to end (inclusive) in a
 *  dump format to output stream.
 *
 *  Return
 *  A dynamically allocated string (make sure to free it)
 */

void reset_memory(Block *block);
/*	Description
 *  Fill the memory block with 0s.
 */

unsigned int read_value_from_memory(Block *block, int start, int size);
/* 
 * Read the value stored in memory, starting at the 'start'th byte, spanning 'size' half bytes.
 * If 'size' is an odd number, the start location is `start` * 2 + 1 half bytes.
 *
 * This function is intended for use with modification records, therefore throws an assertion error
 * if there is an issue with the arguments (which should be checked before this function is called).
 *
 * Return Value
 * the value found
 *
 */

void write_value_to_memory(Block *block, int start, int size, unsigned int value);
/*
 * Store the value in memory, in the same manner as retrieve value.
 *
 * This function is intended for use with modification records, therefore throws an assertion error
 * if there is an issue with the arguments (which should be checked before this function is called).
 *
 */

bool set_load_address(Block *block, unsigned int address);

void set_breakpoint(FILE *out, Block *block, unsigned int address);

int get_breakpoint(Block *block, unsigned int address, unsigned int length);

void clear_breakpoints(Block *block);

void print_breakpoints(Block *block);



#if defined(TEST) && !defined(MEMORY_C)
#undef TEST
#include "memory.c"
#define TEST 0
#endif
