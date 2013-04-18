#include <stdlib.h>

#include "hpc3.h"

// 0x1fb80000 0x1fb8ffff PBUS DMA channel registers
// 0x1fb90000 0x1fb9ffff HD0, HD1, ENET DMA channel registers
// 0x1fba0000 0x1fbaffff Fifo access ports
// 0x1fbb0000 0x1fbbffff General registers
// 0x1fbc0000 0x1fbc7fff HD0 device registers
// 0x1fbc8000 0x1fbcffff HD1 device registers
// 0x1fbd0000 0x1fbd7fff ENET device registers
// 0x1fbd8000 0x1fbdffff PBUS device registers
// 0x1fbe0000 0x1fbfffff Battery backed sram address space

hpc3::hpc3(debug_console *pdc_in, std::string sram) : pdc(pdc_in)
{
	len = 512 * 1024;
	pm = (unsigned char *)malloc(len);

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

	seeq = new seeq_8003_8020(pdc_in);
}

hpc3::~hpc3()
{
	delete seeq;

	delete ser2;
	delete ser1;

	delete pep;
}

void hpc3::read_64b(uint64_t offset, uint64_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read64 %016llx %d", offset, section);

	(((hpc3*)this)->*hpc3::sections_read[section])(S_DWORD, offset & 0xffff, data);
}

void hpc3::write_64b(uint64_t offset, uint64_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write64 %016llx %d: %016llx", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_DWORD, offset & 0xffff, data);
}

void hpc3::read_32b(uint64_t offset, uint32_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read32 %016llx %d", offset, section);

	uint64_t temp = -1;
	(((hpc3*)this)->*hpc3::sections_read[section])(S_WORD, offset & 0xffff, &temp);
	*data = temp;
}

void hpc3::write_32b(uint64_t offset, uint32_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write32 %016llx %d: %08x", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_WORD, offset & 0xffff, data);
}

void hpc3::read_16b(uint64_t offset, uint16_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read16 %016llx %d", offset, section);

	uint64_t temp = -1;
	(((hpc3*)this)->*hpc3::sections_read[section])(S_SHORT, offset & 0xffff, &temp);
	*data = temp;
}

void hpc3::write_16b(uint64_t offset, uint16_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write16 %016llx %d: %04x", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_SHORT, offset & 0xffff, data);
}

void hpc3::read_8b(uint64_t offset, uint8_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 read8 %016llx %d", offset & 0xffff, section);

	uint64_t temp = -1;
	(((hpc3*)this)->*hpc3::sections_read[section])(S_BYTE, offset & 0xffff, &temp);
	*data = temp;

	pdc -> dc_log(" result: %02x", *data);
}

void hpc3::write_8b(uint64_t offset, uint8_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	pdc -> dc_log("HPC3 write8 %016llx %d: %02x", offset, section, data);

	(((hpc3*)this)->*hpc3::sections_write[section])(S_BYTE, offset & 0xffff, data);
}

void hpc3::write_fake(ws_t ws, uint64_t offset, uint64_t data)
{
	if (ws == S_BYTE)
	{
		memory::write_8b(offset, uint8_t(data));
	}
	else if (ws == S_SHORT)
	{
		memory::write_16b(offset, uint16_t(data));
	}
	else if (ws == S_WORD)
	{
		memory::write_32b(offset, uint32_t(data));
	}
	else if (ws == S_DWORD)
	{
		memory::write_64b(offset, data);
	}
}

void hpc3::read_fake(ws_t ws, uint64_t offset, uint64_t *data)
{
	if (ws == S_BYTE)
	{
		uint8_t dummy;
		memory::read_8b(offset, &dummy);
		*data = dummy;
	}
	else if (ws == S_SHORT)
	{
		uint16_t dummy;
		memory::read_16b(offset, &dummy);
		*data = dummy;
	}
	else if (ws == S_WORD)
	{
		uint32_t dummy;
		memory::read_32b(offset, &dummy);
		*data = dummy;
	}
	else if (ws == S_DWORD)
	{
		memory::read_64b(offset, data);
	}
}

void hpc3::section_8_read_pbus_dma(ws_t ws, uint64_t offset, uint64_t *data)
{
	pdc -> dc_log("HPC3 PBUS read not implemented %016llx", offset);
	read_fake(ws, offset, data);
}

void hpc3::section_9_read_hd_enet_channel(ws_t ws, uint64_t offset, uint64_t *data)
{
	pdc -> dc_log("HPC3 HD ENET read not implemented %016llx", offset);
	uint32_t dummy = -1;
	seeq -> read_32b(offset, &dummy);
	*data = dummy;
}

void hpc3::section_a_read_fifo(ws_t ws, uint64_t offset, uint64_t *data)
{
	pdc -> dc_log("HPC3 FIFO read not implemented %016llx", offset);
	read_fake(ws, offset, data);
}

void hpc3::section_b_read_general(ws_t ws, uint64_t offset, uint64_t *data)
{
	if (offset == 0x0004)	// gio.misc, gio64 bus, misc
		*data = gio_misc;	// bit 1: des_endian (1=little), bit 0: en_real_time
	else
	{
		pdc -> dc_log("HPC3 GENERAL read not implemented %016llx", offset);
		read_fake(ws, offset, data);
	}
}

void hpc3::section_c_read_hd_dev_regs(ws_t ws, uint64_t offset, uint64_t *data)
{
	pdc -> dc_log("HPC3 HD DEV read not implemented %016llx", offset);
	read_fake(ws, offset, data);
}

void hpc3::section_d_read_enet_pbus_dev_regs(ws_t ws, uint64_t offset, uint64_t *data)
{
	read_fake(ws, offset, data);

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
			pdc -> dc_log("HPC3 PBUS read %016llx not implemented", offset);
	}
	else
	{
		pdc -> dc_log("HPC3 ENET read %016llx not implemented", offset);
	}
}

void hpc3::section_e_read_sram(ws_t ws, uint64_t offset, uint64_t *data)
{
	uint32_t temp = -1;
	pep -> read_32b(offset, &temp);
	*data = temp;

	pdc -> dc_log("HPC3 SRAM read %016llx: %08lx", temp);
}

void hpc3::section_8_write_pbus_dma(ws_t ws, uint64_t offset, uint64_t data)
{
	pdc -> dc_log("HPC3 PBUS write not implemented %016llx", offset);
	write_fake(ws, offset, data);
}

void hpc3::section_9_write_hd_enet_channel(ws_t ws, uint64_t offset, uint64_t data)
{
	pdc -> dc_log("HPC3 HD ENET write not implemented %016llx", offset);
	seeq -> write_32b(offset, data);
}

void hpc3::section_a_write_fifo(ws_t ws, uint64_t offset, uint64_t data)
{
	pdc -> dc_log("HPC3 FIFO write not implemented %016llx", offset);
	write_fake(ws, offset, data);
}

void hpc3::section_b_write_general(ws_t ws, uint64_t offset, uint64_t data)
{
	if (offset == 0x0004)	// gio.misc, gio64 bus, misc
		gio_misc = data;
	else
	{
		pdc -> dc_log("HPC3 GENERAL write not implemented %016llx", offset);
		write_fake(ws, offset, data);
	}
}

void hpc3::section_c_write_hd_dev_regs(ws_t ws, uint64_t offset, uint64_t data)
{
	pdc -> dc_log("HPC3 HD DEV write not implemented %016llx", offset);

	write_fake(ws, offset, data);
}

void hpc3::section_d_write_enet_pbus_dev_regs(ws_t ws, uint64_t offset, uint64_t data)
{
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
		{
			pdc -> dc_log("HPC3 PBUS write %016llx: %016llx not implemented", offset, data);
			write_fake(ws, offset, data);
		}
	}
	else
	{
		pdc -> dc_log("HPC3 ENET write %016llx: %016llx not implemented", offset, data);
		write_fake(ws, offset, data);
	}
}

void hpc3::section_e_write_sram(ws_t ws, uint64_t offset, uint64_t data)
{
	pdc -> dc_log("HPC3 SRAM write %016llx: %016llx", offset, data);

	pep -> write_32b(offset, data);
}
