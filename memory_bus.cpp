#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "optimize.h"
#include "memory_bus.h"
#include "processor_utils.h"
#include "exceptions.h"

memory_bus::memory_bus(debug_console *pdc_in) : list(NULL), n_elements(0), pdc(pdc_in)
{
	dummy_seg.offset = -1;
	dummy_seg.mask = 0;
	plast_seg = &dummy_seg;
}

memory_bus::~memory_bus()
{
	free(list);
}

void memory_bus::register_memory(uint64_t offset, uint64_t mask, memory *target)
{
	n_elements++;

	list = (memory_segment_t *)realloc(list, n_elements * sizeof(memory_segment_t)); 
	if (!list)
		error_exit("Memory allocation error (register_memory)");

	list[n_elements - 1].offset = offset;
	list[n_elements - 1].mask   = ~mask; // !! ~ of mask!!!
	list[n_elements - 1].target = target;

	pdc -> dc_log("BUS: register %016llx / %016llx", offset, mask);
}

// r/w might overlap segments? FIXME
const memory_segment_t * memory_bus::find_segment(uint64_t offset)
{
	// see if the last used segment is used again (for speed)
	if (likely((offset & plast_seg -> mask) == plast_seg -> offset))
		return plast_seg;

	// if not, (re-)scan segment table
	for(int index = 0; index < n_elements; index++)
	{
		memory_segment_t *psegment = &list[index];

		if ((offset & psegment -> mask) == psegment -> offset)
		{
			plast_seg = psegment;

			return psegment;
		}
	}

	throw processor_exception(offset, -1, -1, PEE_MEM, -1);

	return NULL; // should not be reached
}

void memory_bus::read_64b(uint64_t offset, uint64_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_64b(offset - segment -> offset, data);
}

void memory_bus::write_64b(uint64_t offset, uint64_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_64b(offset - segment -> offset, data);
}

void memory_bus::read_32b(uint64_t offset, uint32_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_32b(offset - segment -> offset, data);
}

void memory_bus::write_32b(uint64_t offset, uint32_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_32b(offset - segment -> offset, data);
}

void memory_bus::read_16b(uint64_t offset, uint16_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_16b(offset - segment -> offset, data);
}

void memory_bus::write_16b(uint64_t offset, uint16_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_16b(offset - segment -> offset, data);
}

void memory_bus::read_8b(uint64_t offset, uint8_t *data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> read_8b(offset - segment -> offset, data);
}

void memory_bus::write_8b(uint64_t offset, uint8_t data)
{
	const memory_segment_t * segment = find_segment(offset);

	segment -> target -> write_8b(offset - segment -> offset, data);
}
