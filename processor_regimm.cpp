#include "processor.h"
#include "processor_utils.h"

void processor::regimm(uint32_t instruction)
{
	uint8_t immediate = instruction & MASK_16B;

	uint8_t rs = (instruction >> 21) & MASK_5B;
	uint8_t rt = (instruction >> 16) & MASK_5B;
	uint8_t function = rt;

	int offset = immediate;
	int b18_signed_offset = untwos_complement_16b(offset) << 2;

	switch(function)
	{
		case 0x01:		// BGEZ
		case 0x03:		// BGEZL
			if (untwos_complement_16b(registers[rs]) >= 0)
				PC += b18_signed_offset;
			break;

		case 0x11:		// BAL / BGEZAL
		case 0x13:		// BGEZALL
			if (rs == 31)
				pdc -> log("i-type / REGIMM / BGEZ using rs 31");

			if (untwos_complement_16b(registers[rs]) >= 0)
			{
				set_register_32b(31, PC + 4);

				PC += b18_signed_offset;
			}
			break;

		default:
			pdc -> log("i-type, opcode 0x01, REGIMM 0x%02x unknown", function);
	}
}
