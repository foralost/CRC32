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
	crc.init_val = (uint32_t)0xFFFFFFFF;
	crc.ref_form = 1;

	char* test = "1";
	size_t s = strlen(test);

	uint64_t calc_val = crc_calc(test, s, &crc);
	printf("%lx\n", calc_val);

	uint64_t rev = 0;
	for(uint64_t i = 0 ; i < 64;i++){
		uint64_t rev_mask = ( (uint64_t) 1 << ( 63 - i ));
		if( calc_val & ((uint64_t) 1 << i) ){
			rev |= rev_mask;
		}
	}




	return 0;
}
