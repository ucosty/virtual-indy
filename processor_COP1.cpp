#include "processor.h"
#include "processor_utils.h"

void processor::COP1(uint32_t instruction)
{
	uint8_t fmt = (instruction >> 21) & MASK_5B;
	uint8_t function = instruction & MASK_5B; // FIXME

	pdc -> dc_log("COP1 fmt %02x", fmt);

	switch(fmt)
	{
		case 0x00:	// MFC1
			break;

		case 0x01:	// DMFC1
			break;

		case 0x02:	// CFC1
			break;

		case 0x04:	// MTC1
			break;

		case 0x05:	// DMTC1
			break;

		case 0x06:	// CTC1
			break;

		case 0x08:	// BC
			switch((fmt >> 16) & 3)
			{
				case 0x00:	// BC1F
					break;

				case 0x01:	// BC1T
					break;

				case 0x02:	// BC1FL
					break;

				case 0x03:	// BC1TL
					break;
			}
			break;

		case 0x10:	// S
			if (function == 0x11)	// MOVCF
			{
				if (instruction & 0x10000)
				{ } // MOVT(fmt)
				else
				{ } // MOVF(fmt)
			}
			else
			{
			}
			break;

		case 0x11:	// D
			if (function == 0x11)	// MOVCF
			{
				if (instruction & 0x10000)
				{ } // MOVT(fmt)
				else
				{ } // MOVF(fmt)
			}
			else
			{
			}
			break;


		case 0x14:	// W
			break;

		case 0x15:	// L
			break;

		default:
			pdc -> dc_log("COP1 unsupported fmt %02x", fmt);
			break;
	}
}

void processor::COP1X(uint32_t instruction)
{
	uint8_t fmt = (instruction >> 21) & MASK_5B;

	pdc -> dc_log("COP1X fmt %02x", fmt);

	switch(fmt)
	{
		default:
			pdc -> dc_log("COP1X unsupported fmt %02x", fmt);
			break;
	}
}
