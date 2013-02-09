#include "processor.h"
#include "processor_utils.h"

void processor::special3(int instruction)
{
	int function = instruction & MASK_6B;
	int sub_function = (instruction >> 6) & MASK_5B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;

	switch(function)
	{
		case 0x20:
			if (sub_function == 0x18)	// SEH
				set_register(rt, sign_extend_16b(registers[rd] & MASK_16B));
			else if (sub_function == 0x10)	// SEB
				set_register(rt, sign_extend_8b(registers[rd] & MASK_8B));
			break;

		default:
			pdc -> log("special3 function %02x not supported", function);
			break;
	}
}
