#include "exceptions.h"

processor_exception::processor_exception(processor_exceptions_type_t type_in, uint64_t address_in)
{
	type = type_in;
	address = address_in;
}

processor_exception::processor_exception(const processor_exception & org)
{
	type = org.get_type();
	address = org.get_address();
}

uint64_t processor_exception::get_address() const
{
	return address;
}

processor_exceptions_type_t processor_exception::get_type() const
{
	return type;
}

const char *processor_exception::get_type_str() const
{
	switch(type)
	{
		case PE_TLB_MISS:
			return "TLB miss";

		case PE_ADDRESS_ERROR:
			return "address alignment error";

		case PE_ARITHMETIC_OVERFLOW:
			return "arithmetic overflow";
	}

	return "??? (!)";
}
