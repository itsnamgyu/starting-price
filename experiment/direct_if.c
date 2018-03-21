#include <stdio.h>

int main(void) {
	for (int i = 0; i < 2; ++i)
		if (i) printf("True\n");
		else printf("No\n");

	for (int i = 0; i < 2; ++i)
		if (i) printf("True\n"); else printf("No\n");

	return 0;
}
