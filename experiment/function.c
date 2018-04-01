#include <stdio.h>

int sum(int a, int b) {
	return a + b;
}

int main(void) {
	int (*f)(int);
	int (*g)(int, int, int);

	f = sum;
	g = sum;

	int ho = f(10);
	printf("%d\n", ho);

	ho = g(10, 20, 30);
	printf("%d\n", ho);

	int (*h)(void) = (int (*)(void)) sum;
	printf("%d\n", h());


	return 0;
}
