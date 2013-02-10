#include "debug.h"
#include "processor.h"
#include "processor_utils.h"

void processor::j_type(uint8_t opcode, uint32_t instruction)
{
	ASSERT(opcode == 2 || opcode == 3);

	if (opcode == 3)	// JAL
		set_register_32b(31, PC);

	int new_PC = ((instruction & MASK_26B) << 2) | (PC & 0xFC000000);

	tick(); // delay slot

	PC = new_PC;
}
