/*
 * \brief  Tests for I2C on omap4 tuna board
 * \author Alexander Tarasikov
 * \date   2012-09-17
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <i2c_session/connection.h>
#include <timer_session/connection.h>
#include <base/printf.h>

enum {
	RGB_LED_ADDR = 0x30,
	RGB_REG_RESET = 0,
	RGB_REG_RESET_VAL = 1,

	RGB_REG_SELECT = 1,
	RGB_REG_ENABLE = 2,

	RGB_REG_RED = 3,
	RGB_REG_GREEN = 4,
	RGB_REG_BLUE = 5,

	RGB_LED_RED = 1,
	RGB_LED_GREEN = 2,
	RGB_LED_BLUE = 4,
	RGB_LED_MASK = 7,
};

using namespace Genode;

int main()
{
	printf("--- Test I2C ---\n");

	static I2C::Connection i2c;
	static Timer::Connection timer;

	#define _w(reg, val) \
	do {\
		if (!i2c.write_byte(RGB_LED_ADDR, reg, val)) {\
			PERR("Failed to write i2c %x %x %x", RGB_LED_ADDR, reg, val);\
			break;\
		}\
	} while (0)

	unsigned color = 5;
	while (1) {
		color &= RGB_LED_MASK;

		_w(RGB_REG_RESET, RGB_REG_RESET_VAL);
		_w(RGB_REG_SELECT, color);
		_w(RGB_REG_ENABLE, color);
		_w(RGB_REG_RED, color & RGB_LED_RED ? 0xff : 0);
		_w(RGB_REG_GREEN, color & RGB_LED_GREEN ? 0xff : 0);
		_w(RGB_REG_BLUE, color & RGB_LED_BLUE ? 0xff : 0);
		timer.msleep(200);

		//color++;
	}

	PINF("I2C Test: done");

	return 0;
}

