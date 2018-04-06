#include <stdio.h>

#define TYPE int

// #Included Generic Header
#define FUNCTION_TEMPLATE(TYPE) \
void print_location_##TYPE (TYPE v) {\
	TYPE n = v;\
	printf("%p\n", (void*) &v);\
}\

#define GENERATE_FUNCTION(type) FUNCTION_TEMPLATE(type)
GENERATE_FUNCTION(TYPE)

// Source Code
int main(void) {
	int ho = 1920;
	
	print_location_int(ho);

	return 0;
}
