#ifndef __MEMORY_BUS__H__
#define __MEMORY_BUS__H__

#include <vector>

#include "debug_console.h"
#include "memory.h"

typedef struct
{
	uint64_t offset_start;
	uint64_t offset_end;
	memory *target;
} memory_segment_t;

class memory_bus
{
private:
	memory_segment_t *list;
	int n_elements;
	int last_index;

	debug_console *pdc;

	const memory_segment_t * find_segment(uint64_t offset);

public:
	memory_bus(debug_console *pdc_in);
	~memory_bus();

	void register_memory(uint64_t offset, uint64_t size, memory *target);

	void read_64b(uint64_t offset, uint64_t *data);
	void write_64b(uint64_t offset, uint64_t data);
	void read_32b(uint64_t offset, uint32_t *data);
	void write_32b(uint64_t offset, uint32_t data);
	void read_16b(uint64_t offset, uint16_t *data);
	void write_16b(uint64_t offset, uint16_t data);
	void read_8b(uint64_t offset, uint8_t *data);
	void write_8b(uint64_t offset, uint8_t data);
};
#endif
