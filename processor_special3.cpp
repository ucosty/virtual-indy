#include "processor.h"
#include "processor_utils.h"

void processor::special3(uint32_t instruction)
{
	uint8_t function = instruction & MASK_6B;
	uint8_t sub_function = (instruction >> 6) & MASK_5B;
	uint8_t rd = get_RD(instruction);
	uint8_t rt = get_RT(instruction);

	switch(function)
	{
		case 0x20:
			if (sub_function == 0x18)	// SEH
				set_register_32b(rt, sign_extend_16b(uint16_t(get_register_32b_unsigned(rd))));
			else if (sub_function == 0x10)	// SEB
				set_register_32b(rt, sign_extend_8b(uint8_t(get_register_32b_unsigned(rd))));
			break;

		default:
			pdc -> dc_log("special3 function %02x not supported", function);
			break;
	}
}
