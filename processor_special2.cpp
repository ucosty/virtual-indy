#include "processor.h"
#include "processor_utils.h"

void processor::special2(int instruction)
{
	int clo = instruction & MASK_6B;
	int rd = (instruction >> 11) & MASK_5B;
	int rt = (instruction >> 16) & MASK_5B;
	int rs = (instruction >> 21) & MASK_5B;

	long long int temp_64b = -1;

	switch(clo)
	{
		case 0x02:		// MUL
			temp_64b = int(registers[rs]) * int(registers[rt]);
			set_register(rd, temp_64b & MASK_32B);
			// LO/HI are said to be unpredictable after this command
			break;

		case 0x1C:		// CLZ
			set_register(rd, count_leading_zeros(32, registers[rs]));
			// FIXME also in rt?
			break;

		case 0x21:		// CLO
			set_register(rd, count_leading_ones(32, registers[rs]));
			// FIXME also in rt?
			break;

		default:
			pdc -> log("special2 clo %02x not supported", clo);
			break;
	}
}
