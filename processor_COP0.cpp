#include "processor.h"
#include "processor_utils.h"

void processor::COP0(uint32_t instruction)
{
	if (IS_BIT_OFF0_SET(25, instruction))	//
	{
		uint8_t function = instruction & MASK_6B;

		switch(function)
		{
#if 0
			case 0x18:	// ERET
				PC = get_C0_register(14, 0);	// EPC (FIXME: sel 0?)
				status_register >>= 4;
				// FIXME
				break;
#endif

            default:
                break;
//				pdc -> dc_log("COP0: function %02x not implemented (1)", function);
		}
	}
	else
	{
		uint8_t function = (instruction >> 21) & MASK_5B;
		uint8_t rd = get_RD(instruction);
		uint8_t rt = get_RT(instruction);

		uint8_t sel = instruction & MASK_3B;

		switch(function)
		{
			case 0x00:	// MFC0
//				DEBUG(pdc -> dc_log("COP0: get register %d,%d", rd, sel));
				set_register_32b(rt, get_C0_register(rd, sel));
				break;

			case 0x04:	// MTC0
//				DEBUG(pdc -> dc_log("COP0: set register %d,%d to value of register %d", rd, sel, rt));
				set_C0_register(rd, sel, get_register_32b_unsigned(rt));
				break;

#if 0
			case 0x08:	// TLBP
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
#endif
			default:
                break;
//				pdc -> dc_log("COP0: function %02x not implemented (0)", function);
		}
	}
}
