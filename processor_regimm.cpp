#include "processor.h"
#include "processor_utils.h"

void processor::regimm(uint32_t instruction)
{
	uint8_t immediate = instruction & MASK_16B;

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t function = (instruction >> 16) & MASK_5B;

	int b18_signed_offset = int16_t(immediate) << 2;

// FIXME 32b registers?!
	switch(function)
	{
		case 0x00:		// BLTZ
		case 0x02:		// BLTZL
			if (get_register_32b_signed(rs) < 0)
			{
				set_register_64b(31, PC + 8);

				PC += b18_signed_offset;
			}
			break;

		case 0x01:		// BGEZ
		case 0x03:		// BGEZL
			if (get_register_32b_signed(rs) >= 0)
				PC += b18_signed_offset;
			break;

		case 0x11:		// BAL / BGEZAL
		case 0x13:		// BGEZALL
			if (rs == 31)
				pdc -> dc_log("i-type / REGIMM / BGEZ using rs 31");

			if (get_register_32b_signed(rs) >= 0)
			{
				set_register_64b(31, PC + 8);

				PC += b18_signed_offset;
			}
			break;

		case 0x10:		// BLTZAL
		case 0x12:		// BLTZALL
			if (get_register_32b_signed(rs) < 0)
			{
				set_register_64b(31, PC + 8);

				PC += b18_signed_offset;
			}
			break;

		default:
			pdc -> dc_log("i-type, opcode 0x01, REGIMM 0x%02x unknown", function);
	}
}
