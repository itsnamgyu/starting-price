#include <stdio.h>

#define DEF_PRINT_LOCATION(TYPE) \
void print_location_##TYPE (TYPE v) {\
	printf("%p\n", (void*) &v);\
}\

DEF_PRINT_LOCATION(int)

int main(void) {
	int ho = 1920;
	
	print_location_int(ho);

	return 0;
}
