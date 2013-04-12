#include "memory.h"
#include "eprom.h"
#include "debug_console.h"

typedef enum { S_BYTE, S_SHORT, S_WORD, S_DWORD } ws_t;

class hpc3 : public memory
{
private:
	debug_console *pdc;
	eprom *pep;

	void section_8_read_pbus_dma(ws_t ws, uint64_t offset, uint64_t *data);
	void section_9_read_hd_enet_channel(ws_t ws, uint64_t offset, uint64_t *data);
	void section_a_read_fifo(ws_t ws, uint64_t offset, uint64_t *data);
	void section_b_read_general(ws_t ws, uint64_t offset, uint64_t *data);
	void section_c_read_hd0_dev_regs(ws_t ws, uint64_t offset, uint64_t *data);
	void section_d_read_hd1_dev_regs(ws_t ws, uint64_t offset, uint64_t *data);
	void section_e_read_enet_dev_regs(ws_t ws, uint64_t offset, uint64_t *data);
	void section_f_read_sram(ws_t ws, uint64_t offset, uint64_t *data);
	void (hpc3::*sections_read[8])(ws_t ws, uint64_t offset, uint64_t *data);

	void section_8_write_pbus_dma(ws_t ws, uint64_t offset, uint64_t data);
	void section_9_write_hd_enet_channel(ws_t ws, uint64_t offset, uint64_t data);
	void section_a_write_fifo(ws_t ws, uint64_t offset, uint64_t data);
	void section_b_write_general(ws_t ws, uint64_t offset, uint64_t data);
	void section_c_write_hd0_dev_regs(ws_t ws, uint64_t offset, uint64_t data);
	void section_d_write_hd1_dev_regs(ws_t ws, uint64_t offset, uint64_t data);
	void section_e_write_enet_dev_regs(ws_t ws, uint64_t offset, uint64_t data);
	void section_f_write_sram(ws_t ws, uint64_t offset, uint64_t data);
	void (hpc3::*sections_write[8])(ws_t ws, uint64_t offset, uint64_t data);

public:
	hpc3(debug_console *pdc_in, std::string sram_file);
	~hpc3();

	uint64_t get_size() const { return 0x100000; }
	uint64_t get_mask() const { return 0xfffff; }

	void read_64b(uint64_t offset, uint64_t *data);
	void read_32b(uint64_t offset, uint32_t *data);
	void read_16b(uint64_t offset, uint16_t *data);
	void read_8b(uint64_t offset, uint8_t *data);
	void write_64b(uint64_t offset, uint64_t data);
	void write_32b(uint64_t offset, uint32_t data);
	void write_16b(uint64_t offset, uint16_t data);
	void write_8b(uint64_t offset, uint8_t data);
};
