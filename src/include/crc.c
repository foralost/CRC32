#include "crc.h"
#define CRC_BITS_UINT64_T 64
#define CRC_16_BIT_VALUES	2^16
#define CRC_8_BIT_VALUES	2^8

uint64_t __crc_calc_tab[(1 << 16)];

uint64_t __crc_gen_mask(uint8_t bits) {
	return (size_t) ((uint64_t) 1 << (uint64_t) (bits - 1));
}

void __crc_calc_tab_init(uint8_t used_bytes, struct crc_info *info) {

	uint64_t reg;
	uint64_t curr_short = 1;
	uint64_t crc_mask = __crc_gen_mask(info->pol_width << 3);

	__crc_calc_tab[0] = 0;
	for (;;) {
		reg = curr_short;
		for (uint8_t i = 0; i < (info->pol_width << 3); i++) {

			if (info->ref_form)
				reg = (reg & 1) ? (reg >> 1) ^ info->polynomial : (reg >> 1);
			else
				reg = (reg & crc_mask) ?
						(reg << 1) ^ info->polynomial : (reg << 1);
		}

		__crc_calc_tab[curr_short] = reg;

		if (used_bytes == 1 && ++curr_short == 0x100) {
			break;
		} else if (used_bytes == 2 && ++curr_short == 0x10000) {
			break;
		}
	}

}

uint8_t __crc_inv_byte(uint8_t val) {
	uint8_t to_ret = 0;

	for (uint8_t i = 0; i < 8; i++) {
		if (val & (1 << i)) {
			to_ret |= 1 << (7 - i);
		}
	}
	return to_ret;
}
uint64_t __crc_inv_value(uint64_t val) {
	size_t to_ret = 0;
	for (uint8_t i = 0; i < 64; i++) {
		if (val & ((uint64_t) 1 << (uint64_t) i)) {
			to_ret |= ((uint64_t) 1 << (63 - i));
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

void __crc_final_round(uint64_t *reg, struct crc_info *info) {
	if (info->xor_val)
		*reg ^= info->xor_val;

	if (info->ref_out) {
		*reg = __crc_inv_value(*reg);
		*reg &= __crc_inv_value(__crc_gen_and_mask(info->pol_width * 8));
		*reg >>= (64 - info->pol_width * 8);
	} else {
		*reg &= __crc_gen_and_mask(info->pol_width * 8);
	}
}
uint64_t crc_calc_table(char *data, size_t length, struct crc_info *info,
		struct crc_tab *tab) {

	__crc_calc_tab_init(tab->bytes_per_iter, info);

	uint16_t top_short = 0;
	uint64_t reg_mask = __crc_gen_mask(info->pol_width * 8);
	uint64_t reg = __crc_gen_init_reg(info, &reg_mask);

	for (size_t i = 0; i < length;) {
		if (info->ref_form) {
			top_short = reg & 0xFFFF;
		} else {
			top_short =
					(tab->bytes_per_iter == 1) ?
							((reg >> ((info->pol_width << 3) - 16)) & 0xFF00)
									>> 8 :
							((reg >> ((info->pol_width << 3) - 16)) & 0xFFFF);
		}

		uint8_t curr_byte = 0;
		// Append register
		uint8_t j;
		for (j = 0; (j < tab->bytes_per_iter) && (i + j) < length; j++) {

			curr_byte =
					info->ref_in ? __crc_inv_byte(data[i + j]) : data[i + j];

			reg = info->ref_form ?
					(reg >> 8) | ((curr_byte) << ((info->pol_width << 3) - 8)) :
					(reg << 8) | (curr_byte);

		}
		i += j;
		reg ^= __crc_calc_tab[top_short];
	}

	for (uint8_t i = 0; i < (info->pol_width >> (tab->bytes_per_iter - 1));
			i++) {

		if (info->ref_form) {
			top_short =
					(tab->bytes_per_iter == 1) ?
							(reg & 0xFF00) >> 8 : reg & 0xFFFF;
		} else {
			top_short =
					(tab->bytes_per_iter == 1) ?
							((reg >> ((info->pol_width << 3) - 16)) & 0xFF00)
									>> 8 :
							((reg >> ((info->pol_width << 3) - 16)) & 0xFFFF);
		}

		reg = info->ref_form ?
				(reg >> (tab->bytes_per_iter << 3)) :
				(reg << (tab->bytes_per_iter << 3));
		reg ^= __crc_calc_tab[top_short];
	}

	__crc_final_round(&reg, info);
	return reg;
}

uint64_t __crc_gen_init_reg(struct crc_info *info, uint64_t *reg_mask) {

	uint64_t reg = 0;
	if (info->init_val) {
		reg = info->init_val;
		if (!info->ref_form) {
			for (uint8_t i = 0; i < (info->pol_width << 3); i++)
				reg = reg & 1 ?
						(uint64_t) (reg ^ info->polynomial) >> (uint64_t) 1
								| *reg_mask :
						(reg >> (uint64_t) 1);
		} else {

			uint64_t temp = 0;
			for (uint64_t i = 0; i < (info->pol_width << 3); i++)
				if (reg & ((uint64_t) 1 << i))
					temp |= (uint64_t) 1 << ((info->pol_width << 3) - i - 1);

			reg &= ~__crc_gen_and_mask(info->pol_width << 3);
			reg |= temp;

			for (uint8_t i = 0; i < (info->pol_width << 3); i++)
				reg = reg & *reg_mask ?
						(uint64_t) (reg ^ info->polynomial) << (uint64_t) 1
								| 1 :
						(reg << (uint64_t) 1);
		}
	} else {
		reg = 0;
	}

	return reg;

}
uint64_t crc_calc(char *data, size_t length, struct crc_info *info) {
// TODO: That black magic with inversed CRC32 when init_val != 0
	uint64_t reg;
	uint64_t reg_mask = __crc_gen_mask(info->pol_width << 3);

	reg = __crc_gen_init_reg(info, &reg_mask);
	for (size_t i = 0; i < length; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			uint8_t curr_bit;
			if (info->ref_in)
				curr_bit = (data[i] & (1 << j)) ? 1 : 0;
			else
				curr_bit = (data[i] & (1 << (7 - j))) ? 1 : 0;

			if (info->ref_form) {
				uint64_t app_val = (uint64_t) curr_bit
						<< (uint64_t) ((info->pol_width << 3) - 1);
				reg = (reg & 1) ?
						((reg >> 1) | app_val) ^ info->polynomial :
						((reg >> 1) | app_val);
			} else {
				reg = (reg & reg_mask) ?
						((reg << 1) | curr_bit) ^ info->polynomial :
						(reg << 1) | curr_bit;
			}
		}
	}

	for (uint8_t i = 0; i < info->pol_width * 8; i++) {
		if (info->ref_form) {
			reg = (reg & 1) ? (reg >> 1) ^ info->polynomial : (reg >> 1);
		} else {
			reg = (reg & reg_mask) ? (reg << 1) ^ info->polynomial : (reg << 1);
		}
	}
	printf("%x\n", __crc_inv_byte(0xb6));
	__crc_final_round(&reg, info);
	return reg;
}

