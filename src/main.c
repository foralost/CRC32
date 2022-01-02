#include <stdio.h>

#include <string.h>
#include "include/crc.h"

int main(int argc, char** argv)
{
	struct crc_info crc;
	memset(&crc, 0, sizeof(struct crc_info));

	struct crc_tab tab_info;
	tab_info.bytes_per_iter = 1;

	crc.pol_width  = 4;
	crc.polynomial = 0xedb88320;
	crc.ref_in = 0;
	crc.ref_out = 0;
	crc.xor_val = -1;
	crc.init_val = 0x00000000ffffffff;
	crc.ref_form = 1;

	char* test = "123456789";
	size_t s = strlen(test);

	printf("%lx\n", crc_calc(test, s, &crc));


	return 0;
}
