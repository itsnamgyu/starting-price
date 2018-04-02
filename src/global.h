#include "history.h"
#include "hashtable.h"
#include "memory.h"

#ifndef GLOBAL_H
#define GLOBAL_H
typedef struct _Global {
	History *history;
	HashTable *table;
	Block *block;
} Global;

extern Global G;
#endif
