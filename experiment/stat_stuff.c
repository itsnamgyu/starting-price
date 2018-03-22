#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main(void) {
	DIR *directory = opendir(".");
	struct dirent *entry;

	while (entry = readdir(directory)) {
		printf("Name: %s\n", entry->d_name);
		printf("\tIs Directory?: %d\n", entry->d_type == DT_DIR);
		printf("\tIs Regular?: %d\n", entry->d_type == DT_REG);
		
		struct stat buffer;
		stat(entry->d_name, &buffer);
		printf("\tIs Executable?: %d\n", buffer.st_mode & S_IXUSR);
	}

	return 0;
}
