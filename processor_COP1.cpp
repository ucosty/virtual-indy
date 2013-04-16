#include "processor.h"
#include "processor_utils.h"

void processor::COP1(uint32_t instruction)
{
	bool tf = false;
	uint8_t fmt = (instruction >> 21) & MASK_5B;

	switch(fmt)
	{
		case 0x08:	// BC
			tf = fmt & 0x10000;
			break;

		case 0x10:	// S
			break;

		case 0x11:	// D
			break;

		case 0x14:	// W
			break;

		default:
			pdc -> dc_log("COP1 unsupported fmt %02x", fmt);
			break;
	}
}
