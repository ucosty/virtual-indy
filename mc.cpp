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
	if (offset == 0x30)	// EEROM
	{
		pdc -> dc_log("EEPROM read");
	}
	else
	{
		pdc -> dc_log("MC read %016llx", offset);
	}
}

void mc::write_32b(uint64_t offset, uint32_t data)
{
	if (offset == 0x30)	// EEROM
	{
		pdc -> dc_log("EEPROM write: %08x", data);
	}
	else
	{
		pdc -> dc_log("MC write @ %016llx: %016llx", offset, data);
	}
}
