#include <stdint.h>

typedef enum { PE_TLB_MISS, PE_ADDRESS_ERROR, PE_ARITHMETIC_OVERFLOW } processor_exceptions_type_t;

class processor_exception
{
private:
	processor_exceptions_type_t type;
	uint64_t address;

public:
	processor_exception(processor_exceptions_type_t type_in, uint64_t address_in);
	processor_exception(const processor_exception & org);
	uint64_t get_address() const;
	processor_exceptions_type_t get_type() const;
	const char *get_type_str() const;
};
