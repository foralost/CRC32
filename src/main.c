#include <stdlib.h>
#include <stdint.h>

#include "include/crc32.h"

int main( int argc, char** argv )
{
	
	unsigned char* test = "123456789";
	size_t test_len = strlen(test);

	uint32_t crc = calc_crc32(test, test_len);
	printf("CRC32: 0x%08X", crc);
	return 0;
}
