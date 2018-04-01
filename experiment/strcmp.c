#include <stdio.h>
#include <string.h>

int *get_invalid_pointer() {
	int a;
	return &a;
}

int main(void) {
	printf("%d\n", strcmp("", "ho"));
	printf("%d\n", strcmp("ho", ""));
	printf("%d\n", strcmp("ho", "hoho"));
	printf("%d\n", strcmp("hoho", "ho"));
	
	printf("%d\n", strcmp(get_invalid_pointer(), "ho"));
	return 0;
}

