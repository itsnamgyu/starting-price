#include "dir.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef TEST
int main(void) {
	fprint_dir(stdout);
}
#endif

void fprint_dir(FILE *out) {
	struct dirent *entry;
	DIR *directory = opendir(".");

	while ((entry = readdir(directory))) {
		char *filename = entry->d_name;
		struct stat buffer;
		stat(filename, &buffer);

		if (filename[0] == '.') continue;

		if (entry->d_type == DT_DIR) {
			fprintf(out, "%s\t", filename);
			continue;
		}

		fprintf(out, "%s", filename);
		if (entry->d_type == DT_DIR) fprintf(out, "\\");
		else if (buffer.st_mode & S_IXUSR) fprintf(out, "*");
		fprintf(out, "\t");
	}

	fprintf(out, "\n");
}
