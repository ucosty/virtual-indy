#include <stdio.h>
#include <string.h>

#include "error.h"
#include "memory_bus.h"
#include "processor_utils.h"

memory_bus::memory_bus()
{
}

memory_bus::~memory_bus()
{
}

void memory_bus::register_memory(uint64_t offset, uint64_t mask, memory *target)
{
	memory_segment_t segment = { offset, mask, target };

	list.push_back(segment);
}

// r/w might overlap segments? FIXME
const memory_segment_t * memory_bus::find_segment(uint64_t offset) const
{
	unsigned int n = list.size();

	for(unsigned int segment = 0; segment < n; segment++)
	{
		const memory_segment_t *psegment = &list.at(segment);

		uint64_t seg_offset = psegment -> offset;
		uint64_t seg_mask   = psegment -> mask ^ MASK_64B;

		if ((offset & seg_mask) == seg_offset)
			return psegment;
	}

	return NULL;
}

bool memory_bus::read_64b(uint64_t offset, uint64_t *data) const
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_64b(offset - segment -> offset, data);
}

bool memory_bus::write_64b(uint64_t offset, uint64_t data)
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_64b(offset - segment -> offset, data);
}

bool memory_bus::read_32b(uint64_t offset, uint32_t *data) const
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_32b(offset - segment -> offset, data);
}

bool memory_bus::write_32b(uint64_t offset, uint32_t data)
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_32b(offset - segment -> offset, data);
}

bool memory_bus::read_16b(uint64_t offset, uint16_t *data) const
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_16b(offset - segment -> offset, data);
}

bool memory_bus::write_16b(uint64_t offset, uint16_t data)
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_16b(offset - segment -> offset, data);
}

bool memory_bus::read_8b(uint64_t offset, uint8_t *data) const
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> read_8b(offset - segment -> offset, data);
}

bool memory_bus::write_8b(uint64_t offset, uint8_t data)
{
	const memory_segment_t * segment = find_segment(offset);
	if (!segment)
		return false;

	return segment -> target -> write_8b(offset - segment -> offset, data);
}
