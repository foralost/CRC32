#include "crc.h"
#define CRC_BITS_UINT64_T 64
#define CRC_BITS_BYTE

uint64_t __crc_gen_mask(uint8_t bits) {
	return (size_t) ((uint64_t)1 << (uint64_t)(bits - 1));
}

uint64_t __crc_inv_value(uint64_t val) {
	size_t to_ret = 0;
	for (uint8_t i = 0; i < 64; i++) {
		if (val & ((uint64_t)1 << (uint64_t)i)) {
			to_ret |= ((uint64_t)1 << (63 - i) );
		}
	}

	return to_ret;
}

uint64_t __crc_gen_and_mask(uint8_t bits) {
	uint64_t to_ret = 1;

	for (uint8_t i = 1; i < bits; i++) {
		to_ret = (to_ret << 1) | 1;
	}

	return to_ret;

}


uint64_t crc_calc(char *data, size_t length, struct crc_info *info) {
	// TODO: That black magic with inversed CRC32 when init_val != 0
	uint64_t reg;
	uint64_t reg_mask;

	if(!info->ref_form)
		reg_mask = __crc_gen_mask(info->pol_width*8) ;

	if (!info->init_val)
		reg = 0;

	for (size_t i = 0; i < length; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			uint8_t curr_bit;
			if (info->ref_in)
				curr_bit = (data[i] & (1 << j) ) ? 1 : 0;
			else
				curr_bit = (data[i] & (1 << ( 7 - j))) ? 1 : 0;

			if(info->ref_form){
				uint64_t app_val = (info->pol_width << 3) - 1;
				reg = (reg & 1) ? ( ( reg >> 1 ) | app_val ) ^ info->polynomial : ( ( reg >> 1 ) | app_val);
			}
			else{
				reg = ( reg & reg_mask ) ? ( ( reg << 1 ) |  curr_bit ) ^ info->polynomial : (reg << 1) | curr_bit;
			}
		}
	}

	for(uint8_t i = 0 ; i < info->pol_width*8; i++){
		if(info->ref_form){
			reg = (reg & 1) ? (reg >> 1) ^ info->polynomial : (reg >> 1);
		}
		else{
			reg = ( reg & reg_mask ) ? (reg << 1) ^ info->polynomial : (reg << 1);
		}
	}


	if (info->xor_val)
			reg ^= info->xor_val;


	if (info->ref_out){
		reg = __crc_inv_value(reg);
		reg &= __crc_inv_value( __crc_gen_and_mask(info->pol_width*8) );
		reg >>= (64 - info->pol_width*8);
	} else{
		reg &= __crc_gen_and_mask(info->pol_width*8);
	}


	return reg;
}



