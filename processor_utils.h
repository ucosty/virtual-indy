#include <stdint.h>

#define MASK_3B		0x07
#define MASK_5B		0x1f
#define MASK_6B		0x3f
#define MASK_8B		0xff
#define MASK_16B	0xffff
#define MASK_26B	0x3FFFFFF
#define MASK_32B	0xffffffff
#define MASK_N(n)	((1 << n) - 1)

#define IS_BIT_OFF0_SET(bit, val)	((val) & (1 << (bit)))
#define SET_BIT(bit, val)		(val) |= (1 << (bit))
#define RESET_BIT(bit, val)		(val) &= ~(1 << (bit))

int twos_complement(int value, uint8_t bits);
int untwos_complement(int value, uint8_t bits);
int count_leading_ones(int n_bits, uint8_t value);
int count_leading_zeros(int n_bits, uint8_t value);
int rotate_right(int value, int n, int width);
uint16_t sign_extend_8b (uint8_t value);	// to 16b
uint32_t sign_extend_16b(uint16_t value);	// to 32b
uint64_t sign_extend_32b(uint32_t value);	// to 64b
int sign_extend(int value, uint8_t bits);

uint32_t make_cmd_I_TYPE(uint8_t rs, uint8_t rt, uint8_t function, int immediate);
uint32_t make_cmd_SPECIAL(uint8_t rt, uint8_t rd, uint8_t sa, uint8_t function, uint8_t extra);
uint32_t make_cmd_J_TYPE(uint8_t function, int target);
uint32_t make_cmd_R_TYPE(uint8_t opcode, uint8_t sa, uint8_t rd, uint8_t rt, uint8_t rs, uint8_t function);
