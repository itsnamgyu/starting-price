#define MEMORY_C

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "memory.h"


static inline void fprint_digit(FILE *out, int i);
// print memory address at start of line in XXXX0 format

static inline void fprint_data_hex(FILE *out, Block *block, int start, int end, int base_index);
// print memory data from start to end in hex

static inline void fprint_data_char(FILE *out, Block *block, int start, int end, int base_index);
// print memory data from start to end in chars


Block *new_memory_block() {
	Block *block = malloc(sizeof(Block));
	for (int i = 0; i < BLOCK_SIZE; ++i)
		block->data[i] = '\0';

	for (int i = 0; i < BLOCK_BUFFER_SIZE; ++i) {
		block->_prebuffer[i] = '\0';
		block->_postbuffer[i] = '\0';
	}
	
	block->current = 0;
	block->breakpoints = new_list();

	for (int i = 0; i < 15; ++i)
		block->registers[i] = 0;

	return block;
}

void set_memory(Block *block, int location, unsigned char value) {
	block->data[location] = value;
}

void fill_memory(Block *block, int start, int end, unsigned char value) {
	if (start < 0) start = 0;
	if (BLOCK_SIZE <= end) end = BLOCK_SIZE - 1;

	for (int i = start; i <= end; i ++)
		block->data[i] = value;
}

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

unsigned int read_value_from_memory(Block *block, int start, int size) {
	const bool odd_size = size % 2 == 1;
	const int n_byte = (size + 1) / 2;
	const unsigned char *lsb = block->data + start + n_byte - 1;

	assert(0 <= size && size <= 8 && "memory read should be within this range");
	assert(0 <= start && start < BLOCK_SIZE && "referenced cells should be within this range");

	unsigned int result = 0;
	for (int i = 0; i < n_byte; ++i) {
		unsigned int digit = 1 << i * 8;
		unsigned char c = *((unsigned char*) (lsb - i));
		if (i == n_byte - 1 && odd_size)
			result += (c % 16) * digit;
		else
			result += c * digit;
	}

	return result;
}

void write_value_to_memory(Block *block, int start, int size, unsigned int value) {
	const bool odd_size = size % 2 == 1;
	const int n_byte = (size + 1) / 2;
	unsigned char * const lsb = block->data + start + n_byte - 1;

	assert(0 <= size && size <= 8 && "memory operands size should be within this range");
	assert(0 <= start && start < BLOCK_SIZE && "referenced cells should be within this range");
	// assert((unsigned int) value / (1 << size * 4) == 0 && "value must fit within the specified size");
	// commented out > allow overflow since a valid program will return it

	for (int i = 0; i < n_byte; ++i) {
		unsigned int digit = 1 << i * 8;
		if (i == n_byte - 1 && odd_size) {
			unsigned char uchar = *(lsb - i);
			uchar = uchar >> 4;
			uchar = uchar << 4;
			uchar += (value / digit) % 16;
			*(lsb - i) = uchar;
		} else {
			*(lsb - i) = (value / digit) % 256;
		}
	}
}

bool set_load_address(Block *block, unsigned int address) {
	if (address > BLOCK_SIZE) {
		fprintf(stderr, "load address exceeds the memory limit\n");
		return false;
	}
	block->load_address = address;
	return true;
}

void set_breakpoint(FILE *out, Block *block, unsigned int address) {
	unsigned int *value = malloc(sizeof(unsigned int));
	*value = address;
	add_to_list(block->breakpoints, value);

	for (LinkedNode *node = block->breakpoints->head->link; node; node = node->link) {
		if (*(unsigned int*) node->value == address) {
			fprintf(out, "[error] breakpoint %X already exists\n", address);
		}
		return;
	}

	fprintf(out, "[ok] create breakpoint %X\n", address);
}

int get_breakpoint(Block *block, unsigned int address, unsigned int length) {
	for (LinkedNode *node = block->breakpoints->head->link; node; node = node->link)
		if (address <= *(unsigned int*) node->value && *(unsigned int*) node->value < address + length) 
			return *(unsigned int*) node->value;
	return -1;
}

void clear_breakpoints(Block *block) {
	free_list(block->breakpoints, free);
	block->breakpoints = new_list();
}


static inline void fprint_digit(FILE *out, int i) {
	int digit_34 = i / 16 % 256;
	int digit_12 = i / 4096; assert(digit_12 < 256);
	fprintf(out, "%02X%02X0 ", digit_12, digit_34);
}

static inline void fprint_data_hex(FILE *out, Block *block, int start, int end, int base_index) {
	for (int i = 0; i < 16; ++i) {
		int index = base_index + i;
		int value = block->data[index];

		if (start <= index && index <= end)
			fprintf(out, "%02X ", value);
		else
			fprintf(out, "   ");
	}
}

static inline void fprint_data_char(FILE *out, Block *block, int start, int end, int base_index) {
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


#ifdef TEST
int main(void) {
	{
		Block *block = new_memory_block();
		for (int i = 0; i < BLOCK_SIZE; ++i)
			set_memory(block, i, i % 256);

		for (int i = 0; i < 256; ++i) {
			assert(read_value_from_memory(block, i, 1) == (unsigned int) i % 16);
			assert(read_value_from_memory(block, i, 2) == (unsigned int) i);
		}

		assert(read_value_from_memory(block, 0, 3) == 1);
		assert(read_value_from_memory(block, 0, 4) == 1);
		assert(read_value_from_memory(block, 0, 5) == 1 * 256 + 2);
		assert(read_value_from_memory(block, 0, 6) == 1 * 256 + 2);
		assert(read_value_from_memory(block, 0, 7) == 1 * 256*256 + 2 * 256 + 3);

		write_value_to_memory(block, 0, 8, 0);
		for (int i = 0; i < 4; ++i)
			assert(block->data[i] == 0);
		assert(block->data[4] != 0);

		write_value_to_memory(block, 0, 7, (unsigned int) 0x89ABCDE);
		assert(block->data[0] == 0x8);
		assert(block->data[1] == 0x9A);
		assert(block->data[2] == 0xBC);
		assert(block->data[3] == 0xDE);

		free(block);
	}

	printf("----------------------------------------\n");
	printf("Automatic tests successful!\n");

	{
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
	}

	return 0;
}
#endif
