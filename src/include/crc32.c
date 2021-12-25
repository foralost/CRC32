/*
 * crc32.c
 *
 *  Created on: 24 gru 2021
 *      Author: foralost
 */

#include "crc32.h"

#define CRC32_BYTE_POSSIBLE_VALUES 255
#define CRC32_LAST_BIT_MASK	0x80000000

#define CRC32_POLYNOMIAL 0x04C11DB7

uint32_t __crc32_table[CRC32_BYTE_POSSIBLE_VALUES] = { 0 };

void __crc32_fill_crc_table() {
	uint32_t reg;
	uint8_t byte = 0;

	for (;;) {
		reg = (byte << 24);

		for (uint8_t byte_size = 0; byte_size < 8; byte_size++) {
			if (reg & CRC32_LAST_BIT_MASK) {
				reg <<= 1;
				reg ^= CRC32_POLYNOMIAL;
			} else {
				reg <<= 1;
			}
		}

		__crc32_table[byte] = reg;
		if (byte == 255)
			break;
		else
			byte++;
	}

}

void __crc32_print_table(uint32_t *arr) {
	printf(" 0x%08X ", arr[0]);

	for (uint32_t i = 1; i < 256; i++) {
		if (!(i % 8))
			printf("\n");

		printf(" 0x%08X ", arr[i]);

	}

	printf("\n");
}

uint8_t inverse_byte(uint8_t byte) {
	uint8_t reflected_byte = 0;

	for (uint8_t i = 0; i < 8; i++) {
		if (byte & (1 << i))
			reflected_byte |= (1 << (7 - i));
	}

	return reflected_byte;
}

uint32_t inverse(uint32_t src) {

	uint32_t toret;

	for (uint8_t i = 0; i < 32; i++) {
		if (src & (1 << i))
			toret |= (1 << (31 - i));
	}

	return toret;
}


uint32_t __crc32_table_approach( unsigned char *data, size_t size) {
	uint32_t reg = -1;
	uint8_t top_byte;

	for (size_t i = 0; i < size; i++) {
		top_byte = (uint8_t)(reg >> 24);
		reg = (reg << 8) | inverse_byte(data[i]);
		reg ^= __crc32_table[top_byte];
	}

	for (size_t i = 0; i < 4; i++) {
		top_byte = (uint8_t) (reg >> 24);
		reg = (reg << 8) ;
		reg ^= __crc32_table[top_byte];
	}

	return inverse(reg) ^ -1;
}

uint32_t calc_crc32(unsigned char *data, size_t size) {
	if (!__crc32_table[1])
		__crc32_fill_crc_table();

	__crc32_print_table(__crc32_table);

	return __crc32_table_approach(data, size);
}

