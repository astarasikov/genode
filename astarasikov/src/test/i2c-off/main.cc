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

using namespace Genode;

int main()
{
	printf("--- Test I2C ---\n");

	static I2C::Connection i2c;
	static Timer::Connection timer;

	uint8_t tmp;
	i2c.read_byte(0x48, 0x25, &tmp);
	i2c.write_byte(0x48, 0x25, tmp | 7);

	PINF("I2C Test: done");

	return 0;
}

