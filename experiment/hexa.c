#include <stdio.h>

int main(void) {
	for (char a = 0; a <= 48; a ++)
		printf("dec: %d, hexa: %X, hexa2: %02X\n", a, a, a);

	return 0;
}
