/*
 * \brief TI TWL stub routines
 * \author Alexander Tarasikov
 * \date   2012-09-19
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/printf.h>
#include <lx_emul.h>

extern "C" {

int twl_i2c_write_u8(u8 mod_no, u8 val, u8 reg) {
	return 0;
}

int twl_i2c_read_u8(u8 mod_no, u8 *val, u8 reg) {
	return 0;
}

int twl6030_interrupt_unmask(u8 bit_mask, u8 offset) {
	return 0;
}

int twl6030_interrupt_mask(u8 bit_mask, u8 offset) {
	return 0;
}

} //extern "C"
