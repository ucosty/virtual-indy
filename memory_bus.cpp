#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "optimize.h"
#include "memory_bus.h"
#include "processor_utils.h"
#include "exceptions.h"

#ifdef _DEBUG
uint64_t index_dist = 0, index_cnt = 0;
#endif

memory_bus::memory_bus() : list(NULL), n_elements(0)
{
}

memory_bus::~memory_bus()
{
	free(list);
}

void memory_bus::register_memory(uint64_t offset, uint64_t size, memory *target)
{
	n_elements++;

	list = (memory_segment_t *)realloc(list, n_elements * sizeof(memory_segment_t)); 
	if (!list)
		error_exit("Memory allocation error (register_memory)");

	list[n_elements - 1].offset_start = offset;
	list[n_elements - 1].offset_end   = offset + size;

	if (list[n_elements - 1].offset_end < list[n_elements - 1].offset_start)
		error_exit("Segment wraps");

	list[n_elements - 1].target = target;

    // pdc -> dc_log("BUS: register %016llx / %016llx", offset, size);

	last_index_i = last_index = 0;
	last_psegment_i = last_psegment = &list[last_index];
}

// r/w might overlap segments? FIXME
const memory_segment_t * memory_bus::find_segment(uint64_t offset)
{
	int start_last_index = last_index;

	for(;;)
	{
		if (offset >= last_psegment -> offset_start && offset < last_psegment -> offset_end)
			return last_psegment;

		last_index++;
		last_index %= n_elements;

		if (last_index == start_last_index)
			break;

		last_psegment = &list[last_index];
	}

    // pdc -> dc_log("%016llx is not mapped", offset);

	throw processor_exception(offset, -1, -1, PEE_MEM, -1);

	return NULL; // should not be reached
}

const memory_segment_t * memory_bus::find_segment_i(uint64_t offset)
{
	int start_last_index = last_index_i;

	for(;;)
	{
		if (offset >= last_psegment_i -> offset_start && offset < last_psegment_i -> offset_end)
			return last_psegment_i;

		last_index_i++;
		last_index_i %= n_elements;

		if (last_index_i == start_last_index)
			break;

		last_psegment_i = &list[last_index_i];
	}

    // pdc -> dc_log("%016llx is not mapped", offset);

	throw processor_exception(offset, -1, -1, PEE_MEM, -1);

	return NULL; // should not be reached
}

void memory_bus::read_64b(uint64_t offset, uint64_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_64b(offset - segment -> offset_start, data);
}

void memory_bus::write_64b(uint64_t offset, uint64_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_64b(offset - segment -> offset_start, data);
}

void memory_bus::read_32b_i(uint64_t offset, uint32_t *data)
{
	const memory_segment_t * segment = find_segment_i(offset);

	segment -> target -> read_32b(offset - segment -> offset_start, data);
}

void memory_bus::read_32b(uint64_t offset, uint32_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_32b(offset - segment -> offset_start, data);
}

void memory_bus::write_32b(uint64_t offset, uint32_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_32b(offset - segment -> offset_start, data);
}

void memory_bus::read_16b(uint64_t offset, uint16_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_16b(offset - segment -> offset_start, data);
}

void memory_bus::write_16b(uint64_t offset, uint16_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_16b(offset - segment -> offset_start, data);
}

void memory_bus::read_8b(uint64_t offset, uint8_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_8b(offset - segment -> offset_start, data);
}

void memory_bus::write_8b(uint64_t offset, uint8_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_8b(offset - segment -> offset_start, data);
}
