#include "history.h"
#include "hashtable.h"
#include "memory.h"
#include "reserved.h"

/*
 * global.h
 *
 * A horrible module that makes global variables just slightly more safe!
 */

typedef struct _Global {
	History *history;
	HashTable *table;
	Block *block;
	ReservedDict *reserved;
} Global;

extern Global G;
