#include <stdbool.h>
#include <stdlib.h>

#include "memory.h"
#include "register.h"
#include "run.h"

typedef union _Operand {
	struct {
		unsigned int memory_operand;
		unsigned int memory_address;
	};
	struct {
		unsigned int register1;
		unsigned int register2;
	};
} Operand;

static void LDA(Block *block, Operand operand);

static void LDL(Block *block, Operand operand);

static void STA(Block *block, Operand operand);

static void STL(Block *block, Operand operand);

static void STX(Block *block, Operand operand);

static void COMP(Block *block, Operand operand);

static void JEQ(Block *block, Operand operand);

static void JLT(Block *block, Operand operand);

static void J(Block *block, Operand operand);

static void JSUB(Block *block, Operand operand);

static void RSUB(Block *block, Operand operand);

static void STCH(Block *block, Operand operand);

static void LDCH(Block *block, Operand operand);

static void LDB(Block *block, Operand operand);

static void LDT(Block *block, Operand operand);

static void RD(Block *block, Operand operand);

static void WD(Block *block, Operand operand);

static void COMPR(Block *block, Operand operand);

static void CLEAR(Block *block, Operand operand);

static void TIXR(Block *block, Operand operand);

static void TD(Block *block, Operand operand);

static void (*op_functions[57]) (Block *block, Operand operand) = {
	LDA ,	NULL,	LDL,	STA ,	STX,	STL ,	NULL,	NULL,
	NULL,	NULL,	COMP,	NULL,	JEQ,	NULL,	JLT ,	J   ,
	NULL,	NULL,	JSUB,	RSUB,	LDCH,	STCH,	NULL,	NULL,
	NULL,	NULL,	LDB ,	NULL,	NULL,	LDT ,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	COMPR,	NULL,	NULL,	NULL,	NULL,	CLEAR,	TIXR,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	RD  ,	WD  ,
	TD  ,
};

static int base_formats[57] = {
	3, 0, 3, 3, 3, 3, 0, 0,
	0, 0, 3, 0, 3, 0, 3, 3,
	0, 0, 3, 1, 3, 3, 0, 0,
	0, 0, 3, 0, 0, 3, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 
	2, 0, 0, 0, 0, 2, 2, 0, 
	0, 0, 0, 0, 0, 0, 3, 3, 
	3,
};

static Operand get_operand(Block *block, unsigned int ext_inst);

static unsigned int get_format(unsigned int ext_inst);

inline static unsigned int get_opcode(unsigned int ext_inst);


void run(Block *block) {
	bool first = true;
	while (1) {
		const unsigned int current_address = block->registers[PC];

		if (current_address >= BLOCK_SIZE) {
			printf("\t%-2s: %012X ", "A", block->registers[A]);
			printf("%-2s: %08X\n", "X", block->registers[X]);
			printf("\t%-2s: %012X ", "L", block->registers[L]);
			printf("%-2s: %012X\n", "PC", block->registers[PC]);
			printf("\t%-2s: %012X ", "B", block->registers[B]);
			printf("%-2s: %012X\n", "S", block->registers[S]);
			printf("\t%-2s: %012X\n", "T", block->registers[T]);
			printf("End program.\n");
			break;
		}

		const unsigned int ext_inst = read_value_from_memory(block, current_address, 8);
		const unsigned int format = get_format(ext_inst);
		const int breakpoint = get_breakpoint(block, current_address, format);

		if (breakpoint >= 0 && !first) {
			printf("\t%-2s: %012X ", "A", block->registers[A]);
			printf("%-2s: %08X\n", "X", block->registers[X]);
			printf("\t%-2s: %012X ", "L", block->registers[L]);
			printf("%-2s: %012X\n", "PC", block->registers[PC]);
			printf("\t%-2s: %012X ", "B", block->registers[B]);
			printf("%-2s: %012X\n", "S", block->registers[S]);
			printf("\t%-2s: %012X\n", "T", block->registers[T]);
			printf("Stop at checkpoint [%X]\n", breakpoint);
			break;
		}

		first = false;
		block->registers[PC] += format;
		Operand operand = get_operand(block, ext_inst);
		op_functions[get_opcode(ext_inst)](block, operand);
	}
}


static Operand get_operand(Block *block, unsigned int ext_inst) {
	Operand operand;
	const unsigned int opcode = ext_inst >> 26;
	const unsigned int format = get_format(ext_inst);
	const unsigned int n = (ext_inst >> 25) & 1;
	const unsigned int i = (ext_inst >> 24) & 1;
	const unsigned int x = (ext_inst >> 23) & 1;
	const unsigned int b = (ext_inst >> 22) & 1;
	const unsigned int p = (ext_inst >> 21) & 1;
	const unsigned int register1 = (ext_inst >> 20) & 0xF;
	const unsigned int register2 = (ext_inst >> 16) & 0xF;
	const unsigned int offset = (ext_inst >> 8) & 0xFFF;
	const unsigned int absolute_address = (ext_inst) & 0xFFFFF;

	unsigned int target_address;
	
	switch (format) {
		case 1:
			// suppress warning
			operand.register1 = 0;
			return operand;
		case 2:
			operand.register1 = register1;
			operand.register2 = register2;
			return operand;
		case 3:
			target_address = offset;
			if (b) {
				target_address += block->registers[B];
			} else if (p) {
				if (target_address >= 0x800)
					target_address = target_address - 0x1000;
				target_address += block->registers[PC];
			}
			break;
		case 4:
			target_address = absolute_address;
			break;
		default:
			fprintf(stderr, "opcode %X is not supported\n", opcode * 4);
			exit(-1);
			return operand;
	}

	if (x) target_address += block->registers[X];

	unsigned int intermediate_address;

	if (!n && i) {
		operand.memory_operand = target_address;
	} else if (n && i) {
		intermediate_address = read_value_from_memory(block, target_address, 6);
		operand.memory_operand = intermediate_address;
		operand.memory_address = target_address;
	} else if (n && !i) {
		intermediate_address = read_value_from_memory(block, target_address, 6);
		operand.memory_address = intermediate_address;
		operand.memory_operand = read_value_from_memory(block, intermediate_address, 6);
	} else {
		operand.register1 = 0; // suppress warnings
	}

	return operand;
}

static unsigned int get_format(unsigned int ext_inst) {
	unsigned int format = base_formats[get_opcode(ext_inst)];
	unsigned int e = (ext_inst >> 20) & 1;
	
	if (format == 3 && e) format = 4;

	return format;
}

inline static unsigned int get_opcode(unsigned int ext_inst) {
	return ext_inst >> 26;
}

static void LDA(Block *block, Operand operand) {
	block->registers[A] = operand.memory_operand;
}

static void LDL(Block *block, Operand operand) {
	block->registers[L] = operand.memory_operand;
}

static void STA(Block *block, Operand operand) {
	write_value_to_memory(block, operand.memory_address, 6, block->registers[A]);
}

static void STX(Block *block, Operand operand) {
	write_value_to_memory(block, operand.memory_address, 6, block->registers[X]);
}

static void STL(Block *block, Operand operand) {
	write_value_to_memory(block, operand.memory_address, 6, block->registers[L]);
}

/*
 * LESS THAN = 0
 * EQUAL TO = 1
 * MORE THAN = 2
 */
static void COMP(Block *block, Operand operand) {
	if (block->registers[A] < operand.memory_operand) {
		block->registers[SW] = 0;
	} else if (block->registers[A] == operand.memory_operand) {
		block->registers[SW] = 1;
	} else {
		block->registers[SW] = 2;
	}
}

static void JEQ(Block *block, Operand operand) {
	if (block->registers[SW] == 1)
		block->registers[PC] = operand.memory_address;
}

static void JLT(Block *block, Operand operand) {
	if (block->registers[SW] == 0)
		block->registers[PC] = operand.memory_address;
}

static void J(Block *block, Operand operand) {
	block->registers[PC] = operand.memory_address;
}

static void JSUB(Block *block, Operand operand) {
	(void) operand; // suppress warnings
	block->registers[L] = block->registers[PC];
	block->registers[PC] = operand.memory_address;
}

static void RSUB(Block *block, Operand operand) {
	(void) operand; // suppress warnings
	block->registers[PC] = block->registers[L];
}

static void STCH(Block *block, Operand operand) {
	write_value_to_memory(block, operand.memory_address, 2, block->registers[A] & 0xFF);
}

static void LDCH(Block *block, Operand operand) {
	block->registers[A] &= 0xFFFF00;
	block->registers[A] |= read_value_from_memory(block, operand.memory_address, 2);
}

static void LDB(Block *block, Operand operand) {
	block->registers[B] = operand.memory_operand;
}

static void LDT(Block *block, Operand operand) {
	block->registers[T] = operand.memory_operand;
}

static void RD(Block *block, Operand operand) {
	(void) block; // suppress warnings
	(void) operand; // suppress warnings
}

static void WD(Block *block, Operand operand) {
	(void) block; // suppress warnings
	(void) operand; // suppress warnings
}

static void COMPR(Block *block, Operand operand) {
	if (block->registers[operand.register1] < block->registers[operand.register2]) {
		block->registers[SW] = 0;
	} else if (block->registers[operand.register1]== block->registers[operand.register2]) {
		block->registers[SW] = 1;
	} else {
		block->registers[SW] = 2;
	}
}

static void CLEAR(Block *block, Operand operand) {
	block->registers[operand.register1] = 0;
}

static void TIXR(Block *block, Operand operand) {
	block->registers[X] ++;
	unsigned int reg = block->registers[operand.register1];

	if (block->registers[X] < reg) {
		block->registers[SW] = 0;
	} else if (block->registers[X] == reg) {
		block->registers[SW] = 1;
	} else {
		block->registers[SW] = 2;
	}
}

static void TD(Block *block, Operand operand) {
	(void) operand; // suppress warnings
	block->registers[SW] = 0;
}


#ifdef TEST
#include "loader.h"

int main(void) {
	FILE *(file[0]);
	file[0] = fopen("2_5.obj", "r");
	Block *block = new_memory_block();
	//set_breakpoint(stderr, block, 0x106B);
	load(file, 1, block);

	run(block);
	/*
	run(block);
	run(block);
	run(block);
	run(block);
	run(block);
	run(block);
	*/

	return 0;
}
#endif
