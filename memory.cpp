#include <string.h>

#include "memory.h"

memory::memory(int size, bool init) : len(size)
{
	if (size < 0)
		throw "memory::memory invalid size"; // format() met size er in FIXME

	pm = new unsigned char[size];

	if (init)
		memset(pm, 0x00, size);
}

memory::memory(unsigned char *p, int size) : pm(p), len(size)
{
	if (size < 0)
		throw "memory::memory invalid size"; // format() met size er in FIXME
}

memory::~memory()
{
	delete [] pm;
}

bool memory::read_64b(uint64_t offset, uint64_t *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset + 8 >= len || offset < 0)
		return false;

	*data = (uint64_t(pm[offset + 0]) << 56) |
		(uint64_t(pm[offset + 1]) << 48) |
		(uint64_t(pm[offset + 2]) << 40) |
		(uint64_t(pm[offset + 3]) << 32) |
		(uint64_t(pm[offset + 4]) << 24) |
		(uint64_t(pm[offset + 5]) << 16) |
		(uint64_t(pm[offset + 6]) <<  8) |
		(uint64_t(pm[offset + 7])      );

	return true;
}

bool memory::read_32b(uint64_t offset, uint32_t *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset + 4 >= len || offset < 0)
		return false;

	*data = (pm[offset + 0] << 24) |
		(pm[offset + 1] << 16) |
		(pm[offset + 2] <<  8) |
		(pm[offset + 3]      );

	return true;
}

bool memory::read_16b(uint64_t offset, uint16_t *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset + 2 >= len || offset < 0)
		return false;

	*data = (pm[offset + 0] <<  8) |
		(pm[offset + 1]      );

	return true;
}

bool memory::read_8b(uint64_t offset, uint8_t *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset >= len || offset < 0)
		return false;

	*data = pm[offset + 0];

	return true;
}

bool memory::write_64b(uint64_t offset, uint64_t data)
{
	// TODO: if big endin processor, put directly in memory

	if (offset + 8 >= len || offset < 0)
		return false;

	pm[offset + 0] = (data >> 56) & 255;
	pm[offset + 1] = (data >> 48) & 255;
	pm[offset + 2] = (data >> 40) & 255;
	pm[offset + 3] = (data >> 32) & 255;
	pm[offset + 4] = (data >> 24) & 255;
	pm[offset + 5] = (data >> 16) & 255;
	pm[offset + 6] = (data >>  8) & 255;
	pm[offset + 7] = (data      ) & 255;

	return true;
}

bool memory::write_32b(uint64_t offset, uint32_t data)
{
	// TODO: if big endin processor, put directly in memory

	if (offset + 4 >= len || offset < 0)
		return false;

	pm[offset + 0] = (data >> 24) & 255;
	pm[offset + 1] = (data >> 16) & 255;
	pm[offset + 2] = (data >>  8) & 255;
	pm[offset + 3] = (data      ) & 255;

	return true;
}

bool memory::write_16b(uint64_t offset, uint16_t data)
{
	// TODO: if big endin processor, put directly in memory

	if (offset + 2 >= len || offset < 0) // + 2 might wrap if size == 64b
		return false;

	pm[offset + 0] = (data >>  8) & 255;
	pm[offset + 1] = (data      ) & 255;

	return true;
}

bool memory::write_8b(uint64_t offset, uint8_t data)
{
	// TODO: if big endin processor, put directly in memory

	if (offset >= len || offset < 0)
		return false;

	pm[offset + 0] = data & 255;

	return true;
}
