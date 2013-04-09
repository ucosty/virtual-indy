#include <stdint.h>

// FIXME 1-3 TLB
// 11 copro missing
// 13- floating point

typedef enum { PEE_MEM = -1, PE_INT = 0, PE_ADDRL = 4, PE_ADDRS = 5, PE_IBUS = 6, PE_DBUS = 7, PE_SYSCALL = 8, PE_BKPT = 9, PE_RI = 10, PE_OVF = 12 } processor_exceptions_type_t;

class processor_exception
{
private:
	uint64_t BadVAddr;
	uint32_t status;
	uint32_t cause;
	uint64_t EPC;

public:
	processor_exception(uint64_t BadVAddr_in, uint32_t status_in, uint8_t ip, uint8_t exccode, uint64_t EPC_in);
	processor_exception(const processor_exception & org);
	uint64_t get_BadVAddr() const;
	uint32_t get_status() const;
	uint32_t get_cause() const;
	processor_exceptions_type_t get_cause_ExcCode() const;
	uint8_t get_ip() const;
	uint64_t get_EPC() const;
};
