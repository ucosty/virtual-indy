#include "memory.h"
#include "eprom.h"
#include "debug_console.h"

class hpc3 : public memory
{
private:
	debug_console *pdc;
	eprom *pep;

	void section_8_read_pbus_dma(uint64_t offset, uint32_t *data);
	void section_9_read_hd_enet_channel(uint64_t offset, uint32_t *data);
	void section_a_read_fifo(uint64_t offset, uint32_t *data);
	void section_b_read_general(uint64_t offset, uint32_t *data);
	void section_c_read_hd_dev_regs(uint64_t offset, uint32_t *data);
	void section_d_read_enet_dev_regs(uint64_t offset, uint32_t *data);
	void section_e_read_sram(uint64_t offset, uint32_t *data);
	void (hpc3::*sections_read[8])(uint64_t offset, uint32_t *data);

	void section_8_write_pbus_dma(uint64_t offset, uint32_t data);
	void section_9_write_hd_enet_channel(uint64_t offset, uint32_t data);
	void section_a_write_fifo(uint64_t offset, uint32_t data);
	void section_b_write_general(uint64_t offset, uint32_t data);
	void section_c_write_hd_dev_regs(uint64_t offset, uint32_t data);
	void section_d_write_enet_dev_regs(uint64_t offset, uint32_t data);
	void section_e_write_sram(uint64_t offset, uint32_t data);
	void (hpc3::*sections_write[8])(uint64_t offset, uint32_t data);

public:
	hpc3(debug_console *pdc_in, std::string sram_file);
	~hpc3();

	uint64_t get_size() const { return 0x80000; }
	uint64_t get_mask() const { return 0x7ffff; }

	void read_32b(uint64_t offset, uint32_t *data);
	void write_32b(uint64_t offset, uint32_t data);
};
