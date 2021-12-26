#include "include/crc.h"
#include <string.h>
int main(int argc, char** argv)
{

	char* test = "123456789";
	size_t s = strlen(test);

	printf("0x%08x\n", calc_crc32(test, s));
	printf("0x%08x\n", calc_crc8(test, s));


	return 0;
}
