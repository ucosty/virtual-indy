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

bool memory::read_32b(int offset, int *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset + 4 >= len || offset < 0)
		return false;

	*data = (pm[offset + 0] << 24) +
		(pm[offset + 1] << 16) +
		(pm[offset + 2] <<  8) +
		(pm[offset + 3]      );

	return true;
}

bool memory::read_16b(int offset, int *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset + 2 >= len || offset < 0)
		return false;

	*data = (pm[offset + 0] <<  8) +
		(pm[offset + 1]      );

	return true;
}

bool memory::read_8b(int offset, int *data) const
{
	// TODO: if big endin processor, read directly from memory

	if (offset >= len || offset < 0)
		return false;

	*data = pm[offset + 0];

	return true;
}

bool memory::write_32b(int offset, int data)
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

bool memory::write_16b(int offset, int data)
{
	// TODO: if big endin processor, put directly in memory

	if (offset + 2 >= len || offset < 0)
		return false;

	pm[offset + 0] = (data >>  8) & 255;
	pm[offset + 1] = (data      ) & 255;

	return true;
}

bool memory::write_8b(int offset, int data)
{
	// TODO: if big endin processor, put directly in memory

	if (offset >= len || offset < 0)
		return false;

	pm[offset + 0] = data & 255;

	return true;
}
