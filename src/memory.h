#define BLOCK_SIZE 1048576
#define MAX_DUMP_LENGTH 1000000

#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

typedef struct _Block {
	unsigned char data[BLOCK_SIZE];
	int current;
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
#endif
