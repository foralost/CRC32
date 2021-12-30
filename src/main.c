#include <stdio.h>

#include <string.h>
#include "include/crc.h"

int main(int argc, char** argv)
{
	struct crc_info crc;
	memset(&crc, 0, sizeof(struct crc_info));

	crc.pol_width  = 1;
	crc.polynomial = 0x39;
	crc.ref_in = 1;
	crc.ref_out = 1;

	char* test = "1";
	size_t s = strlen(test);

	printf("%lx\n", crc_calc(test, s, &crc));


	return 0;
}
