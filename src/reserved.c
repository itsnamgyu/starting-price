#define RESERVED_C
#include "reserved.h"
#include "generic_dict.h"

#include <stdlib.h>

static const int RESERVED_DICT_SIZE = 40;

ReservedDict *new_reserved_dict() {
	ReservedDict *dict = new_dict(RESERVED_DICT_SIZE);
	return dict;
}

void free_reserved_dict(ReservedDict *dict) {
	free_dict(dict, free);
}

void add_to_reserved_dict(ReservedDict *dict, char *key, ReservedType type) {
	ReservedValue *value = malloc(sizeof(ReservedValue));
	value->type = type;
	add_to_dict(dict, key, value);
}

int find_reserved_type(ReservedDict *dict, char *key, ReservedType *type) {
	ReservedValue *value;
	
	if (find_from_dict(dict, key, (void**) &value)) {
		*type = value->type;
		return 1;
	} else {
		return 0;
	}
}
