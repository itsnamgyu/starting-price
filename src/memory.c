#define MEMORY_C
#include "memory.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST
// Manual unit test for this module
// Refer to the README for testing instructions
int main(void) {
	Block *block = new_memory_block();

	for (int i = 0; i < BLOCK_SIZE; ++i)
		set_memory(block, i, i % 256);

	printf("Dump Test\n");
	for (int i = 0; i < 2; ++i)
		dump_memory(stdout, block, -1, -1);

	printf("\nDump Range & Offset Test\n");
	dump_memory(stdout, block, 23, 48);
	for (int i = 0; i < 2; ++i)
		dump_memory(stdout, block, -1, -1);

	printf("\nOverflow Test\n");
	dump_memory(stdout, block, BLOCK_SIZE - 4, -1);
	for (int i = 0; i < 2; ++i)
		dump_memory(stdout, block, -1, -1);

	printf("\nRange & Overflow Test\n");
	dump_memory(stdout, block, 0xFFFD7, 0xFFFE0);
	for (int i = 0; i < 2; ++i)
		dump_memory(stdout, block, -1, -1);

	printf("\nReset Test\n");
	reset_memory(block);
	for (int i = 0; i < 2; ++i)
		dump_memory(stdout, block, -1, -1);

	free(block);

	return 0;
}
#endif

// Make sure to free after use!
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

	if (start < 0) start = 0;
	if (BLOCK_SIZE <= end) end = BLOCK_SIZE - 1;

	for (int i = start; i <= end; i ++)
		block->data[i] = value;
}

static inline void fprint_digit(FILE *out, int i) {
	int digit_34 = i / 16 % 256;
	int digit_12 = i / 4096; assert(digit_12 < 256);
	fprintf(out, "%02X%02X0 ", digit_12, digit_34);
}

static inline void fprint_data_hex(FILE *out, Block *block, 
		                          int start, int end, int base_index) {
	for (int i = 0; i < 16; ++i) {
		int index = base_index + i;
		int value = block->data[index];

		if (start <= index && index <= end)
			fprintf(out, "%02X ", value);
		else
			fprintf(out, "   ");
	}
}

static inline void fprint_data_char(FILE *out, Block *block,
                                   int start, int end, int base_index) {
	const int X20 = 32;
	const int X7E = 126;

	for (int i = 0; i < 16; ++i) {
		int index = base_index + i;
		int value = block->data[index];

		if (index < start || end < index || value < X20 || X7E < value)
			fprintf(out, ".");
		else
			fprintf(out, "%c", value);
	}
}

// Make sure to free after use!
void dump_memory(FILE *out, Block *block, int start, int end) {
	if (start == -1) start = block->current; else assert(start >= 0);
	if (end == -1) end = start + 159;
	if (end >= BLOCK_SIZE) end = BLOCK_SIZE - 1;

	block->current = end + 1;
	if (block->current == BLOCK_SIZE) block->current = 0;
	
	for (int i = start / 16 * 16; i < end / 16 * 16 + 16; i += 16) {
		fprint_digit(out, i);
		fprint_data_hex(out, block, start, end, i);
		fprintf(out, "; ");
		fprint_data_char(out, block, start, end, i);
		fprintf(out, "\n");
	}
}

void reset_memory(Block *block) {
	for (int i = 0; i < BLOCK_SIZE; ++i) block->data[i] = 0;
	block->current = 0;
}
