/*
 * crc32.h
 *
 *  Created on: 24 gru 2021
 *      Author: foralost
 */

#ifndef SRC_INCLUDE_CRC_H_
#define SRC_INCLUDE_CRC_H_
#include <stdint.h>
#include <stddef.h>

struct crc_info{

	uint8_t ref_form;
	uint8_t pol_width;

	uint8_t ref_in;
	uint8_t ref_out;

	uint8_t xor_val;

	uint64_t polynomial;
	uint64_t init_val;
};


struct crc_info_tab{
	uint8_t bytes_per_iter;

};
uint64_t crc_inverse(size_t val);
uint64_t crc_calc(char* data, size_t length, struct crc_info* info);
uint64_t crc_calc_table(char* data, size_t length, struct crc_info* info);
#endif /* SRC_INCLUDE_CRC_H_ */
