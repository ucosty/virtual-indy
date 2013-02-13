#include "mc.h"

mc::mc(debug_console *pdc_in) : pdc(pdc_in)
{
	pm = NULL;
	len = 0;

	refresh_counter = 0;
}

mc::~mc()
{
}

void mc::read_32b(uint64_t offset, uint32_t *data)
{
	if (offset == 0x30 || offset == 0x34)	// EEROM
	{
		pdc -> dc_log("EEROM read");

		// bit 1: endiannes
		// bit 2: register size (32/64)
		// *data = 0x0f;
	}
	else if (offset == 0x48 || offset == 0x4c)	// REF_CTR - refresh counter
	{
		*data = refresh_counter--;
	}
	else if (offset == 0xc0 || offset == 0xc4)	// MEMCFG0
	{
		// BASE0
		// 31 1 served
		// 30 1 number of subbanks
		// 29 1 bank 0/2 valid
		// 28/27/26/25/24 11111 sim size 8M x 36 bits, 2 subbanks
		// 23-16 00100000 (0x8000000 >> 22), 
		// 1111111100100000
		// BASE1
		// 15 1 served
		// 14 1 number of subbanks
		// 13 1 bank 0/2 valid
		// 12/11/10/09/08 11111 sim size 8M x 36 bits, 2 subbanks
		// 07-00 01000000 (0x10000000 >> 22), 
		// 1111111101000000
		// 11111111001000001111111101000000
		*data = 0xFF20FF40;
	}
	else if (offset == 0xc8 || offset == 0xcc)	// MEMCFG1
	{
		// BASE0
		// 31 1 served
		// 30 1 number of subbanks
		// 29 1 bank 0/2 valid
		// 28/27/26/25/24 11111 sim size 8M x 36 bits, 2 subbanks
		// 23-16 01000000 (0x20000000 >> 22), 
		// 1111111101000000
		// BASE1
		// 15 1 served
		// 14 1 number of subbanks
		// 13 1 bank 0/2 valid
		// 12/11/10/09/08 11111 sim size 8M x 36 bits, 2 subbanks
		// 07-00 10000000 (0x30000000 >> 22), 
		// 1111111110000000
		// 11111111010000001111111110000000
		*data = 0xFF40FF80;
	}
	else
	{
		pdc -> dc_log("MC read %016llx", offset);
	}
}

void mc::write_32b(uint64_t offset, uint32_t data)
{
	if (offset == 0x30 || offset == 0x34)	// EEROM
	{
		pdc -> dc_log("EEROM write @ %016llx: %016llx %c%c%c%c", offset, data, data & 8?'1':'0', data&4?'1':'0', data&2?'1':'0', data&1?'1':'0');
	}
	else
	{
		pdc -> dc_log("MC write @ %016llx: %016llx %c%c%c%c", offset, data, data & 8?'1':'0', data&4?'1':'0', data&2?'1':'0', data&1?'1':'0');
	}
}
