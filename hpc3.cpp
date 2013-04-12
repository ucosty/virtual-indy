#include <stdlib.h>

#include "hpc3.h"

hpc3::hpc3(debug_console *pdc_in, std::string sram) : pdc(pdc_in)
{
	pm = NULL;

	sections_read[0] = &hpc3::section_8_read_pbus_dma;
	sections_read[1] = &hpc3::section_9_read_hd_enet_channel;
	sections_read[2] = &hpc3::section_a_read_fifo;
	sections_read[3] = &hpc3::section_b_read_general;
	sections_read[4] = &hpc3::section_c_read_hd_dev_regs;
	sections_read[5] = &hpc3::section_d_read_enet_pbus_dev_regs;
	sections_read[6] = &hpc3::section_e_read_sram;
	sections_read[7] = NULL;

	sections_write[0] = &hpc3::section_8_write_pbus_dma;
	sections_write[1] = &hpc3::section_9_write_hd_enet_channel;
	sections_write[2] = &hpc3::section_a_write_fifo;
	sections_write[3] = &hpc3::section_b_write_general;
	sections_write[4] = &hpc3::section_c_write_hd_dev_regs;
	sections_write[5] = &hpc3::section_d_write_enet_pbus_dev_regs;
	sections_write[6] = &hpc3::section_e_write_sram;

	pep = new eprom(sram, 131072);

	ser1 = new z85c30(pdc_in);
	ser2 = new z85c30(pdc_in);
}

hpc3::~hpc3()
{
	delete ser2;
	delete ser1;

	delete pep;
}

void hpc3::read_64b(uint64_t offset, uint64_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read64 %016llx %d", offset, section);

	(((hpc3*)this)->*hpc3::sections_read[section])(S_DWORD, offset, data);
}

void hpc3::write_64b(uint64_t offset, uint64_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write64 %016llx %d: %016llx", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_DWORD, offset, data);
}

void hpc3::read_32b(uint64_t offset, uint32_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read32 %016llx %d", offset, section);

	uint64_t temp = -1;
	(((hpc3*)this)->*hpc3::sections_read[section])(S_WORD, offset, &temp);
	*data = temp;
}

void hpc3::write_32b(uint64_t offset, uint32_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write32 %016llx %d: %08x", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_WORD, offset, data);
}

void hpc3::read_16b(uint64_t offset, uint16_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read16 %016llx %d", offset, section);

	uint64_t temp = -1;
	(((hpc3*)this)->*hpc3::sections_read[section])(S_SHORT, offset, &temp);
	*data = temp;
}

void hpc3::write_16b(uint64_t offset, uint16_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write16 %016llx %d: %04x", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_SHORT, offset, data);
}

void hpc3::read_8b(uint64_t offset, uint8_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read8 %016llx %d", offset, section);

	uint64_t temp = -1;
	(((hpc3*)this)->*hpc3::sections_read[section])(S_BYTE, offset, &temp);
	*data = temp;

	pdc -> dc_log(" result: %02x", *data);
}

void hpc3::write_8b(uint64_t offset, uint8_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write8 %016llx %d: %02x", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_BYTE, offset, data);
}

void hpc3::section_8_read_pbus_dma(ws_t ws, uint64_t offset, uint64_t *data)
{
	*data = rand() | (uint64_t(rand()) << 32);
}

void hpc3::section_9_read_hd_enet_channel(ws_t ws, uint64_t offset, uint64_t *data)
{
	*data = rand() | (uint64_t(rand()) << 32);
}

void hpc3::section_a_read_fifo(ws_t ws, uint64_t offset, uint64_t *data)
{
	*data = rand() | (uint64_t(rand()) << 32);
}

void hpc3::section_b_read_general(ws_t ws, uint64_t offset, uint64_t *data)
{
	*data = rand() | (uint64_t(rand()) << 32);
}

void hpc3::section_c_read_hd_dev_regs(ws_t ws, uint64_t offset, uint64_t *data)
{
	*data = rand() | (uint64_t(rand()) << 32);
}

void hpc3::section_d_read_enet_pbus_dev_regs(ws_t ws, uint64_t offset_in, uint64_t *data)
{
	*data = rand() | (uint64_t(rand()) << 32); // FIXME

	uint64_t offset = (offset_in - 0xd0000) & 0xfffff;

	if (offset >= 0x8000)
	{
		if ((offset & ~3) == 0x9830)
			*data = ser1 -> ser_command_read();
		else if ((offset & ~3) == 0x9834)
			*data = ser1 -> ser_data_read();
		else if ((offset & ~3) == 0x9838)
			*data = ser2 -> ser_command_read();
		else if ((offset & ~3) == 0x983c)
			*data = ser2 -> ser_data_read();
		else
			pdc -> dc_log("PBUS read %016llx", offset);
	}
	else
	{
		pdc -> dc_log("ENET read %016llx", offset);
	}
}

void hpc3::section_e_read_sram(ws_t ws, uint64_t offset_in, uint64_t *data)
{
	uint64_t offset = (offset_in - 0xe0000) & 0xfffff;

	pdc -> dc_log("SRAM read %016llx", offset);

	uint32_t temp = -1;
	pep -> read_32b(offset, &temp);
	*data = temp;
}

void hpc3::section_8_write_pbus_dma(ws_t ws, uint64_t offset, uint64_t data)
{
}

void hpc3::section_9_write_hd_enet_channel(ws_t ws, uint64_t offset, uint64_t data)
{
}

void hpc3::section_a_write_fifo(ws_t ws, uint64_t offset, uint64_t data)
{
}

void hpc3::section_b_write_general(ws_t ws, uint64_t offset, uint64_t data)
{
}

void hpc3::section_c_write_hd_dev_regs(ws_t ws, uint64_t offset, uint64_t data)
{
}

void hpc3::section_d_write_enet_pbus_dev_regs(ws_t ws, uint64_t offset_in, uint64_t data)
{
	uint64_t offset = (offset_in - 0xd0000) & 0xfffff;

	if (offset >= 0x8000)
	{
		if ((offset & ~3) == 0x9830)
			ser1 -> ser_command_write(uint8_t(data));
		else if ((offset & ~3) == 0x9834)
			ser1 -> ser_data_write(uint8_t(data));
		else if ((offset & ~3) == 0x9838)
			ser2 -> ser_command_write(uint8_t(data));
		else if ((offset & ~3) == 0x983c)
			ser2 -> ser_data_write(uint8_t(data));
		else
			pdc -> dc_log("PBUS write %016llx: %016llx", offset, data);
	}
	else
	{
		pdc -> dc_log("ENET write %016llx: %016llx", offset, data);
	}
}

void hpc3::section_e_write_sram(ws_t ws, uint64_t offset_in, uint64_t data)
{
	uint64_t offset = (offset_in - 0xe0000) & 0xfffff;

	pdc -> dc_log("SRAM write %016llx", offset);

	pep -> write_32b(offset, data);
}
