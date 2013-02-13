#include "mc.h"

mc::mc()
{
}

mc::~mc()
{
}

void mc::read_32b(uint64_t offset, uint32_t *data)
{
	if (offset == 0x30)	// EEROM
	{
	}
}

void mc::write_32b(uint64_t offset, uint32_t data)
{
	if (offset == 0x30)	// EEROM
	{
	}
}
