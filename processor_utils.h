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

int twos_complement(int value, int bits);
int untwos_complement_16b(int value);
int untwos_complement(int value, int bits);
int count_leading_ones(int n_bits, int value);
int count_leading_zeros(int n_bits, int value);
int rotate_right(int value, int n, int width);
int sign_extend_8b(int value);	// to 16b
int sign_extend_16b(int value);	// to 32b
long long int sign_extend_32b(unsigned int value);	// to 64b
int sign_extend(int value, int bits);

int make_cmd_I_TYPE(int rs, int rt, int function, int immediate);
int make_cmd_SPECIAL(int rt, int rd, int sa, int function, int extra);
int make_cmd_J_TYPE(int function, int target);
int make_cmd_R_TYPE(int opcode, int sa, int rd, int rt, int rs, int function);
