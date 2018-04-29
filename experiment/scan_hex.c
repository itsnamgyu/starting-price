#include <stdio.h>

#include "flush_buffer.c"

int main(void) {
	int ho;
	int ret;

	int n;
	printf("\nScan 100X\n");
	ret = scanf("%100X%n", &ho, &n);
	printf("Result: %d\n", ho);
	printf("n: %d\n", n);
	printf("Return: %d\n", ret);
	
	for (int i = 0; i < 100; i++) {
		printf("\nScan Raw: ");
		ret = scanf("%6X", &ho);
		printf("Result: %d\n", ho);
		printf("Return: %d\n", ret);

		printf("Buffer\n");
		flush_buffer();

		printf("\nScan <%%06X>: ");
		ret = scanf("%06X", &ho);
		printf("Result: %d\n", ho);
		printf("Return: %d\n", ret);

		printf("Buffer\n");
		flush_buffer();

		printf("\nScan <HELLO%%06X>: ");
		ret = scanf("HELLO%06X", &ho);
		printf("Return: %d\n", ret);
		if (ret) printf("Result: %d\n", ho);

		printf("Buffer\n");
		flush_buffer();
	}

	return 0;
}
