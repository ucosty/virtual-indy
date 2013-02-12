#ifndef __MEMORY_BUS__H__
#define __MEMORY_BUS__H__

#include <vector>

#include "memory.h"

typedef struct
{
	uint64_t offset;
	uint64_t mask;
	memory *target;
} memory_segment_t;

class memory_bus
{
private:
	memory_segment_t *list;
	int n_elements;

	const memory_segment_t * find_segment(uint64_t offset) const;

public:
	memory_bus();
	~memory_bus();

	void register_memory(uint64_t offset, uint64_t mask, memory *target);

	void read_64b(uint64_t offset, uint64_t *data) const;
	void write_64b(uint64_t offset, uint64_t data);
	void read_32b(uint64_t offset, uint32_t *data) const;
	void write_32b(uint64_t offset, uint32_t data);
	void read_16b(uint64_t offset, uint16_t *data) const;
	void write_16b(uint64_t offset, uint16_t data);
	void read_8b(uint64_t offset, uint8_t *data) const;
	void write_8b(uint64_t offset, uint8_t data);
};
#endif
