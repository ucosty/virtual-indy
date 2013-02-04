#define MASK_5B		0x1f
#define MASK_6B		0x3f
#define MASK_16B	0xffff
#define MASK_26B	0x3FFFFFF
#define MASK_32B	0xffffffff
#define MASK_N(n)	((1 << n) - 1)

#define IS_BIT_OFF0_SET(bit, val)	((val & (1 << bit)) ? 1 : 0)

int twos_complement(int value, int bits);
int untwos_complement(int value, int bits);
int count_leading_ones(int n_bits, int value);
int count_leading_zeros(int n_bits, int value);
int rotate_right(int value, int n, int width);
