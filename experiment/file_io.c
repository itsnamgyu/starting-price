#include <stdio.h>

int main(void) {
	FILE *file = fopen("file0.txt", "r");

	while (!feof(file)) {
		int a;
		char s0[1000];
		char s1[1000];
		int v;

		v = fscanf(file, "%02X", &a);
		printf("%d: %x\n", v, a);
		if (feof(file)) printf("feof 1");

		v = fscanf(file, "%s", s0);
		printf("%d: %s\n", v, s0);
		if (feof(file)) printf("feof 2");

		v = fscanf(file, "%s", s1);
		printf("%d: %s\n", v, s1);
		if (feof(file)) printf("feof 3");
	}

	fclose(file);

	return 0;
}
