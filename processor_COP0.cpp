#include "processor.h"
#include "processor_utils.h"

void processor::COP0(uint32_t instruction)
{
	if (IS_BIT_OFF0_SET(21, instruction))	//
	{
		uint8_t function = instruction & MASK_6B;

		switch(function)
		{
			case 0x18:	// ERET
				PC = get_C0_register(14, 0);	// EPC (FIXME: sel 0?)
				status_register >>= 4;
				// FIXME
				break;

			default:
				pdc -> log("cop0: don't know how to handle function %02x (1)", function);
		}
	}
	else
	{
		uint8_t function = (instruction >> 21) & MASK_5B;
		uint8_t rd = (instruction >> 11) & MASK_5B;
		uint8_t rt = (instruction >> 16) & MASK_5B;

		uint8_t sel = instruction & MASK_3B;

		switch(function)
		{
			case 0x00:	// MFC0
				set_register_32b(rt, get_C0_register(rd, sel));
				break;

			case 0x04:	// MTC0
				set_C0_register(rd, sel, registers[rt]);
				break;

			case 0x0b:	// DI/EI
				if (IS_BIT_OFF0_SET(5, instruction))
				{
					set_register_32b(rt, status_register);
					SET_BIT(SR_EI, status_register);
				}
				else
				{
					set_register_32b(rt, status_register);
					RESET_BIT(SR_EI, status_register);
				}
				break;

			default:
				pdc -> log("cop0: don't know how to handle function %02x (0)", function);
		}
	}
}
