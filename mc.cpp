#include "mc.h"

mc::mc(debug_console *pdc_in) : pdc(pdc_in)
{
	pm = NULL;
	len = 0;
}

mc::~mc()
{
}

void mc::read_32b(uint64_t offset, uint32_t *data)
{
	if (offset == 0x30 || offset == 0x34)	// EEROM
	{
		pdc -> dc_log("EEROM read");
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
