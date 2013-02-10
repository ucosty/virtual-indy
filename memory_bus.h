#ifndef __MEMORY_BUS__H__
#define __MEMORY_BUS__H__

#include <vector>

#include "memory.h"

typedef struct
{
	int offset;
	int mask;
	memory *target;
} memory_segment_t;

class memory_bus
{
private:
	std::vector<memory_segment_t> list;

	const memory_segment_t * find_segment(int offset) const;

public:
	memory_bus();
	~memory_bus();

	void register_memory(int offset, int mask, memory *target);

	bool read_64b(int offset, uint64_t *data) const;
	bool write_64b(int offset, uint64_t data);
	bool read_32b(int offset, uint32_t *data) const;
	bool write_32b(int offset, uint32_t data);
	bool read_16b(int offset, uint16_t *data) const;
	bool write_16b(int offset, uint16_t data);
	bool read_8b(int offset, uint8_t *data) const;
	bool write_8b(int offset, uint8_t data);
};
#endif
