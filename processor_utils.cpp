#include <assert.h>

#include "processor_utils.h"

int untwos_complement(int value, int bits)
{
	assert(bits >= 1 && bits <= 64);

	int sign_bit = 1 << (bits - 1);

	if (sign_bit)
		return (1 << bits) - value;

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
		if ((value & 0x80000000) == 0)
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
