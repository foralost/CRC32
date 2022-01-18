#include <stdio.h>

#include <string.h>
#include "include/crc.h"

int main(int argc, char** argv)
{
	struct crc_info crc;
	memset(&crc, 0, sizeof(struct crc_info));

	struct crc_tab tab_info;
	tab_info.bytes_per_iter = 2;

	crc.pol_width  = 4;
	crc.polynomial = 0x04C11DB7;
	crc.ref_in = 0;
	crc.ref_out = 0;
	crc.xor_val = -1;
	crc.init_val = (uint32_t)0;
	crc.ref_form = 0;

	char* test = "12";
	size_t s = strlen(test);

	uint64_t calc_val = crc_calc_table(test, s, &crc, &tab_info);
	uint64_t calc_val_norm = crc_calc(test, s, &crc);

	printf("Normal: %lX\n", calc_val_norm);
	printf("Tab: %lX\n", calc_val);
	return 0;
}
