#ifndef __MEMORY__H__
#define __MEMORY__H__

#include <stdint.h>

class memory
{
private:
	unsigned char *pm;
	int len;

public:
	memory(int size, bool init);
	memory(unsigned char *p, int len);
	~memory();

	bool read_64b(int offset, uint64_t *data) const;
	bool read_32b(int offset, uint32_t *data) const;
	bool read_16b(int offset, uint16_t *data) const;
	bool read_8b(int offset, uint8_t *data) const;
	bool write_64b(int offset, uint64_t data);
	bool write_32b(int offset, uint32_t data);
	bool write_16b(int offset, uint16_t data);
	bool write_8b(int offset, uint8_t data);
};

#endif
