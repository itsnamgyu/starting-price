#include "history.h"
#include "hashtable.h"
#include "memory.h"
#include "reserved.h"

typedef struct _Global {
	History *history;
	HashTable *table;
	Block *block;
	ReservedDict *reserved;
} Global;

extern Global G;
