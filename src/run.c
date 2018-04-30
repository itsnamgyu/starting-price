#include <stdbool.h>

#include "memory.h"
#include "register.h"
#include "run.h"

typedef union _Operand {
	struct {
		unsigned int memory_operand;
	};
	struct {
		unsigned int register0;
		unsigned int register1;
	};
} Operand;

static void LDA(Operand operand);

static void STA(Operand operand);

static void STL(Operand operand);

static void COMP(Operand operand);

static void JEG(Operand operand);

static void JLT(Operand operand);

static void J(Operand operand);

static void JSUB(Operand operand);

static void STCH(Operand operand);

static void LDB(Operand operand);

static void LDT(Operand operand);

static void RD(Operand operand);

static void WD(Operand operand);

static void COMPR(Operand operand);

static void CLEAR(Operand operand);

static void TIXR(Operand operand);

static void TD(Operand operand);

static void (*op_functions[57]) (Operand operand) = {
	LDA ,	NULL,	NULL,	STA ,	NULL,	STL ,	NULL,	NULL,
	NULL,	NULL,	COMP,	NULL,	JEG,	NULL,	JLT ,	J   ,
	NULL,	NULL,	JSUB,	NULL,	NULL,	STCH,	NULL,	NULL,
	NULL,	NULL,	LDB ,	NULL,	NULL,	LDT ,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	COMPR,	NULL,	NULL,	NULL,	NULL,	CLEAR,	TIXR,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	RD  ,	WD  ,
	TD  ,
};

static int base_formats[57] = {
	3, 0, 0, 3, 0, 3, 0, 0,
	0, 0, 3, 0, 3, 0, 3, 3,
	0, 0, 3, 0, 0, 3, 0, 0,
	0, 0, 3, 0, 0, 3, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 2, 2, 
	3,
};

static Operand get_operand(Block *block, unsigned int ext_inst);

static unsigned int get_format(unsigned int ext_inst);

inline static unsigned int get_opcode(unsigned int ext_inst);


void run(Block *block) {
	bool first = true;
	while (1) {
		const unsigned int current_address = block->registers[pc];
		const unsigned int ext_inst = 
			read_value_from_memory(block, current_address, 8);
		const unsigned int format = get_format(ext_inst);

		int breakpoint = get_breakpoint(block, current_address, format);
		if (breakpoint >= 0 && !first) {
			printf("\t%2s: %012X ", "A", block->registers[A]);
			printf("%2s: %08X\n", "X", block->registers[X]);
			printf("\t%2s: %012X ", "L", block->registers[L]);
			printf("%2s: %012X\n", "PC", block->registers[PC]);
			printf("\t%2s: %012X ", "B", block->registers[B]);
			printf("%2s: %012X\n", "S", block->registers[S]);
			printf("\t%2s: %012X\n", "T", block->registers[T]);
			printf("Stop at checkpoint [%X]\n", breakpoint);
			break;
		}

		first = false;
		block->registers[pc] += format;
		Operand operand = get_operand(block, ext_inst);
		op_functions[get_opcode(ext_inst)](operand);
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
	const unsigned int e = (ext_inst >> 20) & 1;
	const unsigned int reg1 = (ext_inst >> 22) & 0x3;
	const unsigned int reg2 = (ext_inst >> 20) & 0x3;
	const unsigned int offset = (ext_inst >> 8) & 0xFFF;
	const unsigned int absolute_address = (ext_inst) & 0xFFFFF;
	
	switch (format) {
		case 1:
			return operand;
		case 2:
			operand.register1 = reg1;
			operand.register2 = reg2;
			return operand;
		case 3:
			unsigned int target_address = offset;
			unsigned int temp;
			if (b) {
				target_address += block->registers[B];
			} else if (p) {
				target_address += block->registers[PC];
			}
			break;
		case 4:
			unsigned int temp = absolute_address;
			break;
		default:
			printf(stderr, "opcode %X is not supported\n", opcode * 4);
			return operand;
	}

	if (!n && i) {
		operand->memory_operand = target_address;
	} else if (n && i) {
		temp = read_value_from_memory(block, target_address, 6);
		operand->memory_operand = temp;
	} else if (n && !i) {
		temp = read_value_from_memory(block, target_address, 6);
		temp = read_value_from_memory(block, temp, 6);
		operand->memory_operand = temp;
	}

	return operand;
}

static unsigned int get_format(unsigned int ext_inst) {
	unsigned int format = base_formats[get_opcode(ext_inst)];
	unsigned int e = (extended_instruction >> 20) & 1;
	
	if (format == 3 && e) format = 4;

	return format;
}

inline static unsigned int get_opcode(unsigned int ext_inst) {
	return ext_inst >> 26;
}

static void LDA(Operand operand) {
	printf("LDA\n");
}

static void STA(Operand operand) {
	printf("STA\n");
}

static void STL(Operand operand) {
	printf("STL\n");

}
static void COMP(Operand operand) {
	printf("COMP\n");

}
static void JEG(Operand operand) {
	printf("JEG\n");

}
static void JLT(Operand operand) {
	printf("JLT\n");

}
static void J(Operand operand) {
	printf("J\n");

}
static void JSUB(Operand operand) {
	printf("JSUB\n");

}
static void STCH(Operand operand) {
	printf("STCH\n");

}
static void LDB(Operand operand) {
	printf("LDB\n");

}
static void LDT(Operand operand) {
	printf("LDT\n");
}
static void RD(Operand operand) {
	printf("RD\n");
}
static void WD(Operand operand) {
	printf("WD\n");
}
static void COMPR(Operand operand) {
	printf("COMPR\n");
}
static void CLEAR(Operand operand) {
	printf("CLEAR\n");
}
static void TIXR(Operand operand) {
	printf("TIXR\n");
}
static void TD(Operand operand) {
	printf("TD\n");
}
