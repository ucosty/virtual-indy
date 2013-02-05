#include <assert.h>

#include "processor_utils.h"

int untwos_complement(int value, int bits)
{
	assert(bits >= 1 && bits <= 64);

	int sign_bit = 1 << (bits - 1);

	if (value & sign_bit)
		return -((1ll << bits) - value);

	return value;
}

int twos_complement(int value, int bits)
{
	assert(bits >= 1 && bits <= 64);

	if (value >= 0)
		return value;

	return ((value ^ MASK_32B) + 1) & MASK_32B;
}

int count_leading_ones(int n_bits, int value)
{
	assert(n_bits >= 1 && n_bits <= 64);

	int n_bits_1 = 0;

	for(int nr=0; nr<n_bits; nr++)
	{
		if (!(value & 0x80000000))
			break;

		n_bits_1++;

		value <<= 1;
	}

	return n_bits_1;
}

int count_leading_zeros(int n_bits, int value)
{
	assert(n_bits >= 1 && n_bits <= 64);

	int n_bits_0 = 0;

	for(int nr=0; nr<n_bits; nr++)
	{
		if (value & 0x80000000)
			break;

		n_bits_0++;

		value <<= 1;
	}

	return n_bits_0;
}

int rotate_right(int value, int n, int width)
{
	assert(n >= 0 && n <= 64);
	assert(width == 32 || width == 64);

	int right_bits = value & MASK_N(n);

	value >>= n;

	value |= right_bits << (width - n);

	return value;
}

int sign_extend_8b(int value)
{
	if (value & 128)
		return value | 0xffffff00;

	return value;
}

int sign_extend_16b(int value)
{
	if (value & 32768)
		return value | 0xffff0000;

	return value;
}

int sign_extend(int value, int bits)
{
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
	assert(rs >=0 && rs <= 31);
	assert(rt >=0 && rt <= 31);
	assert(function >=0 && function <= 63);
	assert(immediate >=0 && immediate <= 0xffff);

	return (function << 26) |
		(rs << 21) |
		(rt << 16) |
		immediate;
}

int make_cmd_SPECIAL(int rt, int rd, int sa, int function, int extra)
{
	assert(rt >=0 && rt <= 31);
	assert(rd >=0 && rd <= 31);
	assert(sa >=0 && sa <= 31);
	assert(function >=0 && function <= 63);
	assert(extra >=0 && extra <= 31);

	return function |
		(sa << 6) |
		(rd << 11) |
		(rt << 16) |
		(extra << 21);
}

int make_cmd_J_TYPE(int function, int target)
{
	assert(function >=0 && function <= 63);
	assert(target >=0 && target <= MASK_26B);

	return (function << 26) |
		target;
}
