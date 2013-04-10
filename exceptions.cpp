#include "debug.h"
#include "processor_utils.h"
#include "exceptions.h"

processor_exception::processor_exception(uint64_t BadVAddr_in, uint32_t status_in, uint8_t ip, uint8_t ExcCode, uint64_t EPC_in)
{
	ASSERT((ExcCode >=0 && ExcCode <= 31) || ExcCode == 255);
	cause = (ip << 8) | (ExcCode << 2);

	BadVAddr = BadVAddr_in;
	status = status_in;
	EPC = EPC_in;
}

processor_exception::processor_exception(const processor_exception & org)
{
	BadVAddr = org.get_BadVAddr();
	status = org.get_status();
	cause = org.get_cause();
	EPC = org.get_EPC();

	ASSERT((cause & MASK_32B) == cause);
	ASSERT((cause & 128) == 0);
	ASSERT((cause & 3) == 0);
}

uint64_t processor_exception::get_BadVAddr() const
{
	return BadVAddr;
}

uint32_t processor_exception::get_status() const
{
	return status;
}

uint32_t processor_exception::get_cause() const
{
	return cause;
}

processor_exceptions_type_t processor_exception::get_cause_ExcCode() const
{
	return (processor_exceptions_type_t)((cause >> 2) & MASK_5B);
}

uint8_t processor_exception::get_ip() const
{
	return (cause >> 8) & MASK_8B;
}

uint64_t processor_exception::get_EPC() const
{
	return EPC;
}
