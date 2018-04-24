#include <stdio.h>
#include <ctype.h>

void flush_buffer() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) {
		printf("%d: ", c);
		if (isprint(c)) printf("%c", c);
		printf("\n");
	}
}
