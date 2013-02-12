#include "memory.h"

class rom : public memory
{
public:
	rom(std::string file);
	virtual ~rom();

	void write_64b(uint64_t offset, uint64_t data);
	void write_32b(uint64_t offset, uint32_t data);
	void write_16b(uint64_t offset, uint16_t data);
	void write_8b(uint64_t offset, uint8_t data);
};
