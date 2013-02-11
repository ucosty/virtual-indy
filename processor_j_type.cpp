#include "debug.h"
#include "processor.h"
#include "processor_utils.h"

void processor::j_type(uint8_t opcode, uint32_t instruction)
{
	ASSERT(opcode == 2 || opcode == 3);

	if (opcode == 3)	// JAL
		set_register_64b(31, PC);

	uint64_t new_PC = ((instruction & MASK_26B) << 2) | (PC & 0xfffffffffc000000);

	tick(); // delay slot

	PC = new_PC;
}
