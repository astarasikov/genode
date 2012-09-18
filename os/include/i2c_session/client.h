/*
 * \brief  Client-side i2c interface
 * \author Alexander Tarasikov <alexander.tarasikov@gmail.com>
 * \date   2012-09-18
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__I2C_SESSION__CLIENT_H_
#define _INCLUDE__I2C_SESSION__CLIENT_H_

#include <i2c_session/capability.h>
#include <base/rpc_client.h>

namespace I2C {

	struct Session_client : Genode::Rpc_client<Session>
	{
		explicit Session_client(Session_capability session)
		: Genode::Rpc_client<Session>(session) { }

		bool read_byte(Genode::uint8_t address, Genode::uint8_t reg,
			Genode::uint8_t &out)
		{
			return call<Rpc_read_byte>(address, reg, out);
		}

		bool write_byte(Genode::uint8_t address, Genode::uint8_t reg,
			Genode::uint8_t in)
		{
			return call<Rpc_write_byte>(address, reg, in);
		}
	};
}

#endif /* _INCLUDE__I2C_SESSION__CLIENT_H_ */
