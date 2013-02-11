#include <stdint.h>

#define MASK_3B		0x07
#define MASK_5B		0x1f
#define MASK_6B		0x3f
#define MASK_8B		0xff
#define MASK_16B	0xffff
#define MASK_26B	0x3FFFFFF
#define MASK_32B	0xffffffff
#define MASK_64B	0xffffffffffffffff
#define MASK_N(n)	((1 << n) - 1)
#define S32_MAX		0x7fffffff
#define U32_MAX		0x80000000

#define IS_BIT_OFF0_SET(bit, val)	((val) & (1 << (bit)))
#define SET_BIT(bit, val)		(val) |= (1 << (bit))
#define RESET_BIT(bit, val)		(val) &= ~(1 << (bit))

int64_t untwos_complement(uint64_t value, uint8_t bits);
uint64_t twos_complement(int64_t value, uint8_t bits);
uint8_t count_leading_ones(uint8_t n_bits, uint64_t value);
uint8_t count_leading_zeros(uint8_t n_bits, uint64_t value);
uint64_t rotate_right(uint64_t value, uint8_t n, uint8_t width);
uint64_t sign_extend_8b(uint8_t value);
uint64_t sign_extend_16b(uint16_t value);
uint64_t sign_extend_32b(uint32_t value);
int64_t sign_extend(int64_t value, uint8_t bits);
bool test_tc_overflow_32b(int32_t val1, int32_t val2);

uint32_t make_cmd_I_TYPE(uint8_t rs, uint8_t rt, uint8_t function, int immediate);
uint32_t make_cmd_SPECIAL(uint8_t rt, uint8_t rd, uint8_t sa, uint8_t function, uint8_t extra);
uint32_t make_cmd_J_TYPE(uint8_t function, int target);
uint32_t make_cmd_R_TYPE(uint8_t opcode, uint8_t sa, uint8_t rd, uint8_t rt, uint8_t rs, uint8_t function);
