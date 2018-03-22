#define BLOCK_SIZE 1048576
#define MAX_DUMP_LENGTH 1000000
#define X20 32
#define X7E 126

typedef struct _Block {
	unsigned char data[BLOCK_SIZE];
	int current;
} Block;

Block *new_memory_block();

void set_memory(Block *block, int location, unsigned char value);

void fill_memory(Block *block, int start, int end, unsigned char value);
// Inclusive

char *dump_memory(Block *block, int start, int end);

void reset_memory(Block *block);
