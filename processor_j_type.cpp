#include "debug.h"
#include "processor.h"
#include "processor_utils.h"

void processor::j_type(int opcode, int instruction)
{
	ASSERT(opcode == 2 || opcode == 3);

	if (opcode == 3)	// JAL
		set_register(31, PC);

	int new_PC = ((instruction & MASK_26B) << 2) | (PC & 0xFC000000);

	tick(); // delay slot

	PC = new_PC;
}
