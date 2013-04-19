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
	memory_segment_t *list, *last_psegment, *last_psegment_i;
	int n_elements, last_index, last_index_i;

	debug_console *pdc;

	const memory_segment_t * find_segment(uint64_t offset);
	const memory_segment_t * find_segment_i(uint64_t offset);

public:
	memory_bus(debug_console *pdc_in);
	~memory_bus();

	uint64_t get_cur_segment() const { return last_psegment -> offset_start; }
	uint64_t get_cur_segment_i() const { return last_psegment_i -> offset_start; }

	void register_memory(uint64_t offset, uint64_t size, memory *target);

	void read_32b_i(uint64_t offset, uint32_t *data);

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
