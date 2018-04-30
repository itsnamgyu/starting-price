#define LOADER_C

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "generic_list.h"
#include "estab.h"
#include "memory.h"
#include "utility.h"
#include "register.h"


static bool pass1(FILE **files, int n_files, Block *block, Estab *estab);

static bool pass2(FILE **files, int n_files, Block *block, Estab *estab);


bool load(FILE **files, int n_files, Block *block) {
	Estab *estab = new_estab();

	// lengths: 13, 13, 16, 13
	printf("control      symbol       address         length\n");
	printf("section      name\n");
	printf("----------------------------------------------------\n");

	if (!pass1(files, n_files, block, estab)) return false;
	if (!pass2(files, n_files, block, estab)) return false;

	
	return true;
}


static bool pass1(FILE **files, int n_files, Block *block, Estab *estab) {
	int c;
	int cs_address = block->load_address;
	int cs_length;
	unsigned int value;
	char symbol[10];
	char cs_name[10];
	char address_buffer[10];
	char length_buffer[10];

	for (int i = 0; i < n_files; ++i) {
		FILE *file = files[i];
		while (true) { // for each line
			if ((c = fgetc(file)) == EOF) break;

			if (isspace(c)) continue;
			switch (c) {
				case 'H':
					if (fscanf(file, "%6[^\n]", cs_name) != 1) return false;
					if (fscanf(file, "%06X", &value) != 1) return false; // start -> ignore
					if (fscanf(file, "%06X", &cs_length) != 1) return false;

					for (int i = 0; i < 6; ++i) {
						if (cs_name[i] == ' ') {
							cs_name[i] = '\0';
						}
					}

					if (!add_to_estab(estab, cs_name, -1, cs_address)) return false;
					sprintf(address_buffer, "%04X", cs_address);
					sprintf(length_buffer, "%04X", cs_length);
					printf("%-13s%-13s%-16s%-13s\n", cs_name, "", address_buffer, length_buffer);
					break;
				case 'D':
					while (true) {
						if (fscanf(file, "%6[^\n]", symbol) != 1) break; // symbol name
						if (fscanf(file, "%06X", &value) != 1) return false; // address

						for (int i = 0; i < 6; ++i) {
							if (symbol[i] == ' ') {
								symbol[i] = '\0';
							}
						}

						value += cs_address;
						if (!add_to_estab(estab, symbol, -1, value)) return false;

						sprintf(address_buffer, "%04X", value);
						printf("%-13s%-13s%-16s%-13s\n", "", symbol, address_buffer, "");
					}
					break;
				case '.':
				case 'R':
				case 'M':
				case 'T':
				case 'E':
					for (int c = 'a'; c != EOF && c != '\n'; c = fgetc(file));
					break;
			}
		}
		cs_address += cs_length;
		rewind(file);
	}

	return true;
}

static bool pass2(FILE **files, int n_files, Block *block, Estab *estab) {
	int c;
	int cs_address = block->load_address;
	int cs_length;
	unsigned int value;
	unsigned int length;
	unsigned int address;
	int offset;
	char sign;
	unsigned int reference;
	char symbol[10];
	char cs_name[10];

	for (int i = 0; i < n_files; ++i) {
		FILE *file = files[i];
		while (true) { // for each line
			if ((c = fgetc(file)) == EOF) break;

			if (isspace(c)) continue;
			switch (c) {
				case 'H':
					if (fscanf(file, "%6[^\n]", cs_name) != 1) return false;
					if (fscanf(file, "%06X", &value) != 1) return false; // start -> ignore
					if (fscanf(file, "%06X", &cs_length) != 1) return false;
					break;
				case 'R':
					while (true) {
						if (fscanf(file, "%02X", &address) != 1) break;
						if (fscanf(file, "%6s", symbol) != 1) return false;
						if (!assign_reference_number_to_symbol(estab, symbol, address)) return false;
					}
					break;
				case 'T':
					if (fscanf(file, "%06X", &address) != 1) return false;
					if (fscanf(file, "%02X", &length) != 1) return false;

					address += cs_address;
					for (int i = 0; i < (int) length; ++i) {
						if (fscanf(file, "%02X", &value) != 1) return false;
						set_memory(block, address + i, (unsigned char) value);
					}
					break;
				case 'M':
					if (fscanf(file, "%06X", &address) != 1) return false;
					if (fscanf(file, "%02X", &length) != 1) return false;
					if (fscanf(file, "%c", &sign) != 1) return false;
					if (fscanf(file, "%02X", &reference) != 1) return false;

					address += cs_address;
					value = read_value_from_memory(block, address, length);
					
					if (reference == 1) {
						offset = cs_address;
					} else {
						offset = find_from_estab_by_reference_number(estab, reference);
						if (offset == -1) return false;
					}

					if (sign == '-') value -= offset;
					else if (sign == '+') value += offset;
					else return false;

					write_value_to_memory(block, address, length, value);
					break;
				case 'E':
					if (fscanf(file, "%06X", &address) != 1) break;
					block->registers[PC] = address + cs_address;
					block->start_address = address + cs_address;
					break;
				case 'D':
				case '.':
					for (int c = 'a'; c != EOF && c != '\n'; c = fgetc(file));
					break;
			}
		}
		cs_address += cs_length;
	}

	char length_buffer[10];
	sprintf(length_buffer, "%04X", cs_address - block->load_address);
	printf("----------------------------------------------------\n");
	printf("%-13s%-13s%-16s%-13s\n", "", "", "total length", length_buffer);

	return true;
}


#ifdef TEST
int main(void) {
	FILE *(file[3]);
	file[0] = fopen("proga.obj", "r");
	file[1] = fopen("progb.obj", "r");
	file[2] = fopen("progc.obj", "r");
	Block *block = new_memory_block();
	load(file, 3, block);

	dump_memory(stdout, block, 0, 1000);
	
	return 0;
}
#endif
