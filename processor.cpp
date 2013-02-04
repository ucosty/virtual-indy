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

	if (!pbm -> read_32b(pc, &instruction))
	{
		// processor exception FIXME
	}

	pc += 4;

	int opcode = (instruction >> 26) & 0x3f;

	switch(opcode)
	{
		default:
			// throw invalid
	}
}
