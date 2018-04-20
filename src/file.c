#include "file.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef TEST
// Manual unit test for this module
// Refer to the README for testing instructions
int main(void) {
	printf(">> Print dir:\n");
	fprint_dir(stdout);

	printf(">> Print file.h:\n");
	if (!fprint_file(stdout, "file.h"))
		printf("Error opening file!\n");

	printf(">> Print hoho.h:\n");
	if (!fprint_file(stdout, "hoho.h"))
		printf("Error opening file!\n");
}
#endif

void fprint_dir(FILE *out) {
	struct dirent *entry;
	DIR *directory = opendir(".");

	// for each file in current directory
	while ((entry = readdir(directory))) {
		char *filename = entry->d_name;
		struct stat buffer;
		stat(filename, &buffer);

		if (filename[0] == '.') continue;

		if (entry->d_type == DT_DIR) {
			fprintf(out, "%s/\t", filename);
			continue;
		}

		fprintf(out, "%s", filename);

		// print \ at the end if it's a directory
		if (entry->d_type == DT_DIR) fprintf(out, "\\");

		// print * at the end if it's an executable
		else if (buffer.st_mode & S_IXUSR) fprintf(out, "*");

		fprintf(out, "\t");
	}
	fprintf(out, "\n");
}

int fprint_file(FILE *out, char *filename) {
	FILE *file;
	if (!(file = fopen(filename, "r"))) return 0;
	if (feof(file)) return 0;

	// Print all characters from 'file' to output stream
	char c;
	while ((c = fgetc(file)) != EOF)
		fprintf(out, "%c", c);
	return 1;
}
