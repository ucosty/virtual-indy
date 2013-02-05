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

	bool read_32b(int offset, int *data) const;
	bool write_32b(int offset, int data);
	bool read_16b(int offset, int *data) const;
	bool write_16b(int offset, int data);
	bool read_8b(int offset, int *data) const;
	bool write_8b(int offset, int data);
};
#endif
