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
	sections_read[5] = &hpc3::section_d_read_enet_dev_regs;
	sections_read[6] = &hpc3::section_e_read_sram;

	sections_write[0] = &hpc3::section_8_write_pbus_dma;
	sections_write[1] = &hpc3::section_9_write_hd_enet_channel;
	sections_write[2] = &hpc3::section_a_write_fifo;
	sections_write[3] = &hpc3::section_b_write_general;
	sections_write[4] = &hpc3::section_c_write_hd_dev_regs;
	sections_write[5] = &hpc3::section_d_write_enet_dev_regs;
	sections_write[6] = &hpc3::section_e_write_sram;

	pep = new eprom(sram, 131072);
}

hpc3::~hpc3()
{
	delete pep;
}

void hpc3::read_32b(uint64_t offset, uint32_t *data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	(((hpc3*)this)->*hpc3::sections_read[section])(offset, data);
}

void hpc3::write_32b(uint64_t offset, uint32_t data)
{
	uint8_t section = ((offset >> 16) & 0x0f) - 0x08;

	(((hpc3*)this)->*hpc3::sections_write[section])(offset, data);
}

void hpc3::section_8_read_pbus_dma(uint64_t offset, uint32_t *data)
{
	*data = rand();
}

void hpc3::section_9_read_hd_enet_channel(uint64_t offset, uint32_t *data)
{
	*data = rand();
}

void hpc3::section_a_read_fifo(uint64_t offset, uint32_t *data)
{
	*data = rand();
}

void hpc3::section_b_read_general(uint64_t offset, uint32_t *data)
{
	*data = rand();
}

void hpc3::section_c_read_hd_dev_regs(uint64_t offset, uint32_t *data)
{
	*data = rand();
}

void hpc3::section_d_read_enet_dev_regs(uint64_t offset, uint32_t *data)
{
	*data = rand();
}

void hpc3::section_e_read_sram(uint64_t offset_in, uint32_t *data)
{
	uint64_t offset = (offset_in & 0xfffff) - 0xe0000;

	pep -> read_32b(offset, data);
}

void hpc3::section_8_write_pbus_dma(uint64_t offset, uint32_t data)
{
}

void hpc3::section_9_write_hd_enet_channel(uint64_t offset, uint32_t data)
{
}

void hpc3::section_a_write_fifo(uint64_t offset, uint32_t data)
{
}

void hpc3::section_b_write_general(uint64_t offset, uint32_t data)
{
}

void hpc3::section_c_write_hd_dev_regs(uint64_t offset, uint32_t data)
{
}

void hpc3::section_d_write_enet_dev_regs(uint64_t offset, uint32_t data)
{
}

void hpc3::section_e_write_sram(uint64_t offset_in, uint32_t data)
{
	uint64_t offset = (offset_in & 0xfffff) - 0xe0000;

	pep -> write_32b(offset, data);
}
