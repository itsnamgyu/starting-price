#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Recall this definition
struct Block {
	unsigned char data[BLOCK_SIZE];
	int current;
}
*/

int main(void) {
	Block *block = new_memory_block();

	for (int i = 0; i < BLOCK_SIZE; ++i)
		set_memory(block, i, i % 256);

	for (int i = 0; i < 400; ++i) {
		char *dump_string = dump_memory(block, -1, -1);
		printf("%s", dump_string);
		free(dump_string);
	}

	char *dump_string = dump_memory(block, 23, 48);
	printf("%s", dump_string);
	free(dump_string);

	for (int i = 0; i < 5; ++i) {
		char *dump_string = dump_memory(block, -1, -1);
		printf("%s", dump_string);
		free(dump_string);
	}

	free(block);

	return 0;
}

Block *new_memory_block() {
	Block *block = malloc(sizeof(Block));
	for (int i = 0; i < BLOCK_SIZE; ++i)
		block->data[i] = '\0';
	block->current = 0;
	
	return block;
}

void set_memory(Block *block, int location, unsigned char value) {
	block->data[location] = value;
}

void fill_memory(Block *block, int start, int end, unsigned char value) {
	// Inclusive, Namgyu's exception rules

	if (start < 0) start = 0; // TODO: check start, end exception rules
	if (BLOCK_SIZE <= end) end = BLOCK_SIZE - 1;

	for (int i = start; i <= end; i ++)
		block->data[i] = value;
}

char *dump_memory(Block *block, int start, int end) {
	char *string = malloc(sizeof(char) * MAX_DUMP_LENGTH);
	char *full_string = string;

	if (start == -1) start = block->current; else assert(start > 0);
	if (end == -1) end = start + 159;
	if (end >= BLOCK_SIZE) end = BLOCK_SIZE - 1;

	block->current = end + 1;
	
	for (int i = start / 16 * 16; i < end / 16 * 16 + 16; i += 16) {
		int digit_34 = i / 16 % 256;
		int digit_12 = i / 4096; assert(digit_12 < 256);
		string += sprintf(string, "%02X%02X0 ", digit_12, digit_34);
		// the above expession appends a formatted string to 'string'

		for (int j = 0; j < 16; ++j) {
			int index = i + j;
			int value = block->data[index];

			if (start <= index && index <= end)
				string += sprintf(string, "%02X ", value);
			else
				string += sprintf(string, "   ");
		}
				
		string += sprintf(string, "; ");

		for (int j = 0; j < 16; ++j) {
			int index = i + j;
			int value = block->data[index];

			if (i + j < start || end < i + j || value < X20 || X7E < value)
				string += sprintf(string, ". ");
			else
				string += sprintf(string, "%c ", value);
		}

		string += sprintf(string, "\n");
	}

	return full_string;
}
