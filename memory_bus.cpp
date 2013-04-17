#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "optimize.h"
#include "memory_bus.h"
#include "processor_utils.h"
#include "exceptions.h"

memory_bus::memory_bus(debug_console *pdc_in) : list(NULL), n_elements(0), last_index(0), pdc(pdc_in)
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

	pdc -> dc_log("BUS: register %016llx / %016llx", offset, size);
}

// r/w might overlap segments? FIXME
const memory_segment_t * memory_bus::find_segment(uint64_t offset)
{
	// if not, (re-)scan segment table
	for(int index = 0; index < n_elements; index++)
	{
		int cur_index = (index + last_index) % n_elements;

		memory_segment_t *psegment = &list[cur_index];

		if (offset >= psegment -> offset_start && offset < psegment -> offset_end)
		{
			last_index = cur_index;

			return psegment;
		}
	}

	pdc -> dc_log("%016llx is not mapped", offset);

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
