#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "estab.h"
#include "memory.h"
#include "utility.h"


static bool pass1(FILE **files, int n_files, Block *block, Estab *estab);

static bool pass2(FILE **files, int n_files, Block *block, Estab *estab);


bool load(FILE **files, int n_files, Block *block) {
	Estab *estab = new_estab();

	// lengths: 13, 13, 16, 13
	printf("control      symbol       address         length\n");
	printf("section      name\n");
	printf("----------------------------------------------------\n");

	if (!pass1(files, n_files, block, estab)) return false;
	for (int i = 0; i < n_files; ++i) rewind(files[i]);
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
			if ((c = fgetc(file)) == EOF) return false;

			if (isspace(c)) continue;
			switch (c) {
				case 'H':
					if (!fscan_fixed_length_string(file, 6, cs_name)) return false;
					if (!fscan_fixed_length_hex(file, 6, &value)) return false; // start -> ignore
					if (!fscan_fixed_length_hex(file, 6, &value)) return false; // length
					add_to_estab(estab, cs_name, -1, value);
					cs_length = value;
					sprintf(address_buffer, "%04X", cs_address);
					sprintf(length_buffer, "%04X", cs_length);
					printf("%-13s%-13s%-16s%-13s\n", cs_name, "", address_buffer, length_buffer);
					break;
				case 'D':
					while (true) {
						if (!fscan_fixed_length_string(file, 6, symbol)) break;
						if (!fscan_fixed_length_hex(file, 6, &value)) return false;

						add_to_estab(estab, symbol, -1, value);
						sprintf(address_buffer, "%04X", value);
						printf("%-13s%-13s%-16s%-13s\n", "", symbol, address_buffer, "");
					}
					break;
				case 'M':
					for (int c = 'a'; c != EOF && c != '\n'; c = fgetc(file));
					break;
				case 'E':
					for (int c = 'a'; c != EOF && c != '\n'; c = fgetc(file));
					break;
			}
			
			cs_address += cs_length;
		}
	}
}

static bool pass2(FILE **files, int n_files, Block *block, Estab *estab) {
}


#ifdef TEST
int main(void) {
	FILE *file = fopen("a.obj", "r");
	load(&file, 1, new_memory_block());

	return 0;
}
#endif
