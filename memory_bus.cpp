#include <string.h>

#include "memory_bus.h"

memory_bus::memory_bus()
{
}

memory_bus::~memory_bus()
{
}

void memory_bus::register_memory(int offset, int mask, memory *target)
{
	memory_segment_t segment = { offset, mask, target };

	list.push_back(segment);
}

// r/w might overlap segments? FIXME
memory_segment_t * memory_bus::find_segment(int offset)
{
	for(unsigned int segment = 0; segment < list.size(); segment++)
	{
		memory_segment_t *psegment = &list.at(segment);

		if ((offset & psegment -> mask) == psegment -> offset)
			return psegment;
	}

	return NULL;
}

bool memory_bus::read_32b(int offset, int *data)
{
	memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_32b(offset - segment -> offset, data);
}

bool memory_bus::write_32b(int offset, int data)
{
	memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_32b(offset - segment -> offset, data);
}

bool memory_bus::read_16b(int offset, int *data)
{
	memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_16b(offset - segment -> offset, data);
}

bool memory_bus::write_16b(int offset, int data)
{
	memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_16b(offset - segment -> offset, data);
}

bool memory_bus::read_8b(int offset, int *data)
{
	memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_8b(offset - segment -> offset, data);
}

bool memory_bus::write_8b(int offset, int data)
{
	memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_8b(offset - segment -> offset, data);
}
