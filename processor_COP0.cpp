#include "processor.h"
#include "processor_utils.h"

void processor::COP0(int instruction)
{
	if (IS_BIT_OFF0_SET(21, instruction))	//
	{
		int function = instruction & MASK_6B;

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
		int function = (instruction >> 21) & MASK_5B;
		int rd = (instruction >> 11) & MASK_5B;
		int rt = (instruction >> 16) & MASK_5B;

		int sel = instruction & MASK_3B;

		switch(function)
		{
			case 0x00:	// MFC0
				set_register(rt, get_C0_register(rd, sel));
				break;

			case 0x04:	// MTC0
				set_C0_register(rd, sel, registers[rt]);
				break;

			case 0x0b:	// DI/EI
				if (IS_BIT_OFF0_SET(5, instruction))
				{
					set_register(rt, status_register);
					SET_BIT(SR_EI, status_register);
				}
				else
				{
					set_register(rt, status_register);
					RESET_BIT(SR_EI, status_register);
				}
				break;

			default:
				pdc -> log("cop0: don't know how to handle function %02x (0)", function);
		}
	}
}
