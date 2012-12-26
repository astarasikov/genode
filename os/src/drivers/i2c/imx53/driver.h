/*
 * \brief  I2C Driver for i.MX53
 * \author Alexander Tarasikov
 * \author Nikolay Golikov
 * \date   2012-11-26
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

/* local includes */
#include "i2c.h"

namespace I2C {

	using namespace Genode;
	class Driver;
}


class I2C::Driver
{
public:
	private:

		/* memory map */
		enum {
			I2C1_MMIO_BASE = 0x63fc8000,
			I2C2_MMIO_BASE = 0x63fc4000,
			I2C3_MMIO_BASE = 0x53fec000,

			I2C_MMIO_SIZE = 0x4000,
		};

		/* i2c sub system registers */
		Io_mem_connection *_i2c_mmio;
		addr_t             _i2c_base;
		struct Imx53_I2C  *_i2c;

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

			default:
				PERR("i.MX53 I2C: Unknown bus number %d", bus_number);
				throw("Unknown bus");
			}
			PDBG("I2C base address: 0x%lx\n", io_base);

			/**
			 * Dynamic creation of Io_mem_connection and Imx53_I2C
			 * used because base address of i2c bus depends on
			 * bus number configuration parameter.
			 */
			_i2c_mmio = new (env()->heap()) Io_mem_connection(io_base, I2C_MMIO_SIZE);
			_i2c_base = env()->rm_session()->attach(_i2c_mmio->dataspace());
			_i2c = new (env()->heap()) Imx53_I2C(_i2c_base);
		}

		~Driver()
		{
			destroy(env()->heap(), _i2c);
			env()->rm_session()->detach(_i2c_base);
			destroy(env()->heap(), _i2c_mmio);
		}

		bool read(Genode::uint8_t address,	Genode::uint8_t reg,
		          Genode::uint8_t *out, Genode::uint8_t len)
		{
			if (_i2c->i2c_read(address, reg, out, len) < 0) {
				PERR("i.MX53 i2c: read failed\n");
				return false;
			}
			return true;
		}

		bool write(Genode::uint8_t address, Genode::uint8_t reg,
		           Genode::uint8_t *in, Genode::uint8_t len)
		{
			if (_i2c->i2c_write(address, reg, in, len) < 0) {
				PERR("i.MX53 i2c: write failed\n");
				return false;
			}
			return true;
		}

};
