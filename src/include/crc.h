/*
 * crc32.h
 *
 *  Created on: 24 gru 2021
 *      Author: foralost
 */

#ifndef SRC_INCLUDE_CRC_H_
#define SRC_INCLUDE_CRC_H_
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

uint32_t calc_crc32(char* data, size_t size);
uint8_t calc_crc8(char* data, size_t size);
uint8_t calc_crc8_tab(char* data, size_t size);
#endif /* SRC_INCLUDE_CRC_H_ */
