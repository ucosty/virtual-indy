#include <string>

#include "rom.h"
#include "exceptions.h"
#include "utils.h"

rom::rom(std::string file)
{
	load_file(file, &pm, &len);
}

rom::~rom()
{
}

void rom::write_64b(uint64_t offset, uint64_t data)
{
	
	throw processor_exception(offset, -1, 0, PEE_RMEMS, -1);
}

void rom::write_32b(uint64_t offset, uint32_t data)
{
	throw processor_exception(offset, -1, 0, PEE_RMEMS, -1);
}

void rom::write_16b(uint64_t offset, uint16_t data)
{
	throw processor_exception(offset, -1, 0, PEE_RMEMS, -1);
}

void rom::write_8b(uint64_t offset, uint8_t data)
{
	throw processor_exception(offset, -1, 0, PEE_RMEMS, -1);
}
