int twos_complement(int value, int bits)
{
	int sign_bit = 1 << (bits - 1);

	if (sign_bit)
		return (1 << bits) - value;

	return value;
}

int count_leading_ones(int n_bits, int value)
{
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
