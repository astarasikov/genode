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
#include <i2c_session/i2c_session.h>
#include <cap_session/connection.h>
#include <dataspace/client.h>
#include <base/printf.h>
#include <base/sleep.h>
#include <root/component.h>
#include <os/attached_ram_dataspace.h>
#include <os/static_root.h>
#include <os/config.h>

/* local includes */
#include "driver.h"

namespace I2C {

	using namespace Genode;
	class Session_component;
}

class I2C::Session_component : public Genode::Rpc_object<I2C::Session, Session_component>
{
	private:
		Driver &_driver;

		enum { IO_BUFFER_SIZE = 4096 };

		Attached_ram_dataspace _io_buffer;

	public:

		Session_component(Driver &driver)
		:
			_driver(driver),
			_io_buffer(env()->ram_session(), IO_BUFFER_SIZE)
		{ }

		/************************************
		 ** I2C::Session interface **
		 ************************************/

		virtual bool read(Genode::uint8_t address, Genode::uint8_t reg,
		                  Genode::uint8_t *out, Genode::size_t len)
		{
			if (len > IO_BUFFER_SIZE) {
				PERR("len (%d) exceeds the buffrer size (%d)", len,
				     IO_BUFFER_SIZE);
				return false;
			}

			Genode::uint8_t *out_buf = _io_buffer.local_addr<uint8_t>();
			return _driver.read(address, reg, out_buf, len);
		}

		virtual bool write(Genode::uint8_t address, Genode::uint8_t reg,
		                   Genode::uint8_t* in, Genode::size_t len)
		{
			if (len > IO_BUFFER_SIZE) {
				PERR("len (%d) exceeds the buffrer size (%d)", len,
				     IO_BUFFER_SIZE);
				return false;
			}

			Genode::uint8_t *in_buf = _io_buffer.local_addr<uint8_t>();
			return _driver.write(address, reg, in_buf, len);
		}

		Dataspace_capability dataspace(void) { return _io_buffer.cap(); }

};


int main(int, char **)
{
	using namespace I2C;

	PINF("i.MX53 I2C driver");

	unsigned bus_number = 0;
	config()->xml_node().attribute("bus_number").value(&bus_number);
	PDBG("I2C Bus: %u", bus_number);

	static Driver driver(bus_number);

	/*
	 * Initialize server entry point
	 */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "i2c_ep");

	/*
	 * Let the entry point serve the i2c session and root interfaces
	 */
	static Session_component i2c_session(driver);
	static Static_root<I2C::Session> i2c_root(ep.manage(&i2c_session));

	/*
	 * Announce service
	 */
	env()->parent()->announce(ep.manage(&i2c_root));

	Genode::sleep_forever();
	return 0;
}

