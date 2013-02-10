#include "debug.h"

#include "processor_utils.h"

int untwos_complement_16b(int value)
{
	if (value & 32768)
		return -(MASK_16B - value + 1);

	return value;
}

int untwos_complement(int value, int bits)
{
	ASSERT(bits >= 1 && bits <= 64);

        if (IS_BIT_OFF0_SET(bits - 1, value))
                return -((1ll << bits) - value); // FIXME fails with 64 bit?

	return value;
}

int twos_complement(int value, int bits)
{
	ASSERT(bits >= 1 && bits <= 64);

	if (value >= 0)
		return value;

	int mask = MASK_N(bits);

	return value & mask;
}

int count_leading_ones(int n_bits, int value)
{
	ASSERT(n_bits >= 1 && n_bits <= 64);

	int n_bits_1 = 0;
	int mask = 1 << (n_bits - 1);

	for(int nr=0; nr<n_bits; nr++)
	{
		if (!(value & mask))
			break;

		n_bits_1++;

		value <<= 1;
	}

	return n_bits_1;
}

int count_leading_zeros(int n_bits, int value)
{
	ASSERT(n_bits >= 1 && n_bits <= 64);

	int n_bits_0 = 0;
	int mask = 1 << (n_bits - 1);

	for(int nr=0; nr<n_bits; nr++)
	{
		if (value & mask)
			break;

		n_bits_0++;

		value <<= 1;
	}

	return n_bits_0;
}

int rotate_right(int value, int n, int width)
{
	ASSERT(n >= 0 && n <= 64);
	ASSERT(width >= 0 && width <= 64);

	int right_bits = value & MASK_N(n);

	value >>= n;

	value |= right_bits << (width - n);

	return value;
}

int sign_extend_8b(int value)
{
	ASSERT(value >= 0 && value <= 255);

	if (value & 128)
		return value | 0xffffff00;

	return value;
}

int sign_extend_16b(int value)
{
	ASSERT(value >= 0 && value <= 65535);

	if (value & 32768)
		return value | 0xffff0000;

	return value;
}

long long int sign_extend_32b(unsigned int value)
{
	ASSERT(value >= 0 && value <= MASK_32B);

	if (IS_BIT_OFF0_SET(31, value))
		return value | 0xffffffff00000000;

	return value;
}

int sign_extend(int value, int bits)
{
	ASSERT(bits >= 1 && bits <= 64);

	int mask = 1 << (bits - 1);

	if (value & mask)
	{
		while(bits <= 32)
		{
			value |= mask;

			bits++;
			mask <<= 1;
		}
	}

	return value;
}

int make_cmd_I_TYPE(int rs, int rt, int function, int immediate)
{
	ASSERT(rs >=0 && rs <= 31);
	ASSERT(rt >=0 && rt <= 31);
	ASSERT(function >=0 && function <= 63);
	ASSERT(immediate >=0 && immediate <= 0xffff);

	return (function << 26) |
		(rs << 21) |
		(rt << 16) |
		immediate;
}

int make_cmd_SPECIAL(int rt, int rd, int sa, int function, int extra)
{
	ASSERT(rt >=0 && rt <= 31);
	ASSERT(rd >=0 && rd <= 31);
	ASSERT(sa >=0 && sa <= 31);
	ASSERT(function >=0 && function <= 63);
	ASSERT(extra >=0 && extra <= 31);

	return function |
		(sa << 6) |
		(rd << 11) |
		(rt << 16) |
		(extra << 21);
}

int make_cmd_J_TYPE(int function, int target)
{
	ASSERT(function >=0 && function <= 63);
	ASSERT(target >=0 && target <= MASK_26B);

	return (function << 26) |
		target;
}

int make_cmd_R_TYPE(int opcode, int sa, int rd, int rt, int rs, int function)
{
	return (opcode << 26) |
		(rs << 21) |
		(rt << 16) |
		(rd << 11) |
		(sa << 6) |
		function;
}
