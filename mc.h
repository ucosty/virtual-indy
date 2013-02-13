#include "memory.h"

class mc : public memory
{
public:
	mc();
	~mc();

	uint64_t get_size() { return 0x100000; }
	uint64_t get_mask() { return  0xfffff; }

	void read_32b(uint64_t offset, uint32_t *data);
	void write_32b(uint64_t offset, uint32_t data);
};
