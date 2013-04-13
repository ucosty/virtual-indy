#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "z85c30.h"

z85c30::z85c30(debug_console *pdc_in)
{
	pdc = pdc_in;

	memset(d, 0x00, sizeof d);
	cr = 0;

	tx_full = false;
}

z85c30::~z85c30()
{
}

void z85c30::ser_command_write(uint8_t data)
{
	if (cr == 0)
	{
		uint8_t crc_reset_code = data >> 6;
		uint8_t command_code = (data >> 3) & 7;

		if (command_code == 1) // high point bit
			cr = data & 15;
		else
			cr = data & 7;

		pdc -> dc_log("serial: select register %d, command code %d, crc reset code %d", cr, command_code, crc_reset_code);
	}
	else
	{
		pdc -> dc_log("serial: write %02x to register %d", data, cr);

		if (cr == 8)
		{
			pdc -> dc_term("%c", data);
			pdc -> dc_log("serial OUTPUT: %c", data);
			// tx_full = true; not neccessary
		}

		cr = 0;
	}
}

uint8_t z85c30::ser_command_read()
{
	uint8_t ret = -1;

	if (cr == 0)
	{
		ret = 0x28 | (tx_full ? 0 : 4); // 00101t00 CTS/DCD, t: set when empty

		tx_full = false;
	}

	cr = 0;

	pdc -> dc_log("serial: read from register %d: %02x", cr, ret);

	return ret;
}

void z85c30::ser_data_write(uint8_t data)
{
	ASSERT(cr < 16);

	pdc -> dc_term("%c", data);

	pdc -> dc_log("serial: write DATA %02x", data);
}

uint8_t z85c30::ser_data_read()
{
	pdc -> dc_log("serial: read DATA");

	return 0;
}
