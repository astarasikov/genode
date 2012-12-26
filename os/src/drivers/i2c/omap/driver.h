/*
 * \brief  I2C Driver for OMAP4
 * \author Alexander Tarasikov <tarasikov@ksyslabs.org>
 * \date   2012-12-26
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <io_mem_session/connection.h>
#include <timer_session/connection.h>
#include <util/mmio.h>

#include "i2c.h"

namespace I2C {
	using namespace Genode;
	class Driver;
}

//TODO:
//1. Move common omap4 defines
//	[base/include/drivers/board/panda_a2.h -> b/i/d/cpu/omap/omap4.h]
//2. Include omap4.h and use I/O defines from it

class I2C::Driver
{
	public:
	private:

		/* memory map */
		enum {
			L4_PER_BASE = 0x48000000,

			I2C1_MMIO_BASE = L4_PER_BASE + 0x70000,
			I2C2_MMIO_BASE = L4_PER_BASE + 0x72000,
			I2C3_MMIO_BASE = L4_PER_BASE + 0x60000,
			I2C4_MMIO_BASE = L4_PER_BASE + 0x350000,
			
			I2C_MMIO_SIZE = 0x1000,
		};

		/* display sub system registers */
		Io_mem_connection *_i2c_mmio;
		addr_t _i2c_base;
		struct Omap4_I2C *_i2c;

	public:
		Driver(unsigned bus_number)
		{
			Genode::addr_t io_base = 0;

			switch (bus_number) {
				case 1:
					io_base = I2C1_MMIO_BASE;
					break;
				case 2:
					io_base = I2C2_MMIO_BASE;
					break;
				case 3:
					io_base = I2C3_MMIO_BASE;
					break;
				case 4:
					io_base = I2C4_MMIO_BASE;
					break;

				default:
					PERR("OMAP I2C: Unknown bus number %d", bus_number);
					throw("Unknown bus");
			}
			_i2c_mmio = new (env()->heap())
				Io_mem_connection (io_base, I2C_MMIO_SIZE);
			_i2c_base = env()->rm_session()->attach(_i2c_mmio->dataspace());
			_i2c = new (env()->heap()) Omap4_I2C(_i2c_base);
		}

		~Driver() {
			destroy(env()->heap(), _i2c);
			env()->rm_session()->detach(_i2c_base);
			destroy(env()->heap(), _i2c_mmio);
		}

		bool read(Genode::uint8_t address,	Genode::uint8_t reg,
			Genode::uint8_t *out, Genode::uint8_t len)
		{
			if (_i2c->i2c_read(address, reg, 1, out, len) < 0) {
				PERR("omap i2c: read failed");
				return false;
			}
			return true;
		}

		bool write(Genode::uint8_t address, Genode::uint8_t reg,
			Genode::uint8_t *in, Genode::uint8_t len)
		{
			if (_i2c->i2c_write(address, reg, 1, in, len) < 0) {
				PERR("omap i2c: write failed");
				return false;
			}
			return true;
		}
};
