#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#define MASK_32B        0xffffffff

double get_ts()
{
        struct timeval ts;

        gettimeofday(&ts, NULL);

        return double(ts.tv_sec) + double(ts.tv_usec) / 1000000.0;
}

int main(int argc, char *argv[])
{
	for(int it=0; it<2; it++)
	{
		double s1 = get_ts();
		int d1 = 0;
		for(int n=0; n<2000000000; n++)
			d1 += n ^ MASK_32B;
		printf("%f\n", get_ts() - s1);

		double s2 = get_ts();
		int d2 = 0;
		for(int n=0; n<2000000000; n++)
			d2 += ~n;
		printf("%f\n", get_ts() - s2);
	}

	return 0;
}
