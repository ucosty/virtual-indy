#include "debug_console.h"
#include "memory.h"

class mc : public memory
{
private:
	debug_console *pdc;

public:
	mc(debug_console *pdc_in);
	~mc();

	uint64_t get_size() const { return 0x100000; }
	uint64_t get_mask() const { return  0xfffff; }

	void read_32b(uint64_t offset, uint32_t *data);
	void write_32b(uint64_t offset, uint32_t data);
};
