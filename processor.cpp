#include <string.h>

#include "processor.h"

processor::processor(memory_bus *pmb_in) : pmb(pmb_in)
{
	memset(registers, 0x00, sizeof registers);

	pc = 0;
}

processor::~processor()
{
}

void processor::tick()
{
	int instruction = -1;

	if (!pmb -> read_32b(pc, &instruction))
	{
		// processor exception FIXME
	}

	pc += 4;

	int opcode = (instruction >> 26) & 0x3f;

	switch(opcode)
	{
		case 2:		// J-type
		case 3:		// J-type
			j_type(opcode, instruction);
			break;

		default:
			// throw invalid
			break;
	}
}

void processor::j_type(int opcode, int instruction)
{
	if (opcode == 3)	// JAL
		registers[31] = pc;

	pc = instruction & 0x3FFFFFF | (pc & 0x3c);
}
