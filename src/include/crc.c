#include "crc.h"

#define CRC8_POLYNOMIAL 0x07
#define CRC8_MASK		0x80
#define CRC8_INIT		0

uint8_t __inverse_byte(uint8_t byte) {
	uint8_t to_ret = 0;
	for (uint8_t i = 0; i < 8; i++) {
		if (byte & (1 << i))
			to_ret |= (1 << (7 - i));
	}

	return to_ret;
}

uint32_t __inverse_uint(uint32_t val) {
	uint32_t to_ret = 0;
	for (uint32_t i = 0; i < 32; i++) {
		if (val & (1 << i))
			to_ret |= (1 << (31 - i));
	}

	return to_ret;
}



uint8_t calc_crc8(char *data, size_t size) {
	uint8_t reg = CRC8_INIT;
	for (size_t i = 0; i < size; i++) {
		uint8_t curr_byte = data[i];
		for (uint8_t j = 0; j < 8; j++) {
			uint8_t curr_bit = (curr_byte & (1 << (7 - j))) ? 1 : 0;
			if (reg & CRC8_MASK) {
				reg = (reg << 1) | curr_bit;
				reg ^= CRC8_POLYNOMIAL;
			} else {
				reg = (reg << 1) | curr_bit;
			}
		}

	}

	for (uint8_t i = 0; i < 8; i++) {
		if (reg & CRC8_MASK) {
			reg = (reg << 1) ^ CRC8_POLYNOMIAL;
		} else {
			reg <<= 1;
		}

	}

	return reg;
}

#define CRC32_POLYNOMIAL 0x04C11DB7
uint32_t calc_crc32(char *data, size_t size) {
	uint32_t reg = -1;
	for (size_t i = 0; i < size; i++) {
		uint8_t curr_byte = (data[i]);
		for (uint8_t j = 0; j < 8; j++) {
			uint8_t curr_bit = (curr_byte & (1 << j)) ? 1 : 0;
			if (reg & 0x80000000) {
				reg = (reg << 1) | curr_bit;
				reg ^= CRC32_POLYNOMIAL;
			} else {
				reg = (reg << 1) | curr_bit;
			}
		}

	}

	for (uint8_t i = 0; i < 32; i++) {
		if (reg & 0x80000000) {
			reg = (reg << 1) ^ CRC32_POLYNOMIAL;
		} else {
			reg <<= 1;
		}

	}
	return __inverse_uint(reg) ^ -1;
}
