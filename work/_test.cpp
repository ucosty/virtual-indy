#include <stdint.h>
#include <stdio.h>

#include "../processor_utils.h"

int main(int argc, char *argv[])
{
	printf("%02x\n", -3 & 0xff);
	printf("%d\n", -3 & 0xff);

	printf("%d\n", (1 << 16));

printf("%d\n", 123 ^ 255);

printf("%d\n", sizeof(int));

	printf("%d\n", char(0x80));
	printf("%d\n", short(0x8000));
	printf("%d\n", int(0x80000000));

	printf("%d\n", (-3) << 2);
	printf("%d\n", ((-3) << 2) & 3);

printf("%04x\n", -1230 & 0xffff);

int16_t i16 = -1;
int bla = i16;
printf("%d\n", bla);

	int m = 0x12345678;
	printf("%08x %08x\n", m ^ MASK_32B, ~m);

	int32_t i = -9;
	uint64_t iu = i;
	printf("%016llx\n", iu);

	uint64_t blap = 0x80000000;
	blap <<= 1;
	printf("%016llx\n", blap);

	uint32_t blap2 = 0x80000000;
	printf("%016llx\n", blap2 << 1);

	return 0;
}
