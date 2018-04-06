#include <stdio.h>

#define TYPE int

// #Included Generic Header
#define FUNCTION_TEMPLATE(TYPE) \
void TYPE##_print_location (TYPE v) {\
	printf("%p\n", (void*) &v);\
}\

#define GENERATE_FUNCTION(type) FUNCTION_TEMPLATE(type)
GENERATE_FUNCTION(TYPE)

// Source Code
int main(void) {
	int ho = 1920;
	
	int_print_location(ho);

	return 0;
}
