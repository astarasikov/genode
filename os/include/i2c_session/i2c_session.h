/*
 * \brief  I2C session interface
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

#ifndef _INCLUDE__I2C_SESSION__I2C_SESSION_H_
#define _INCLUDE__I2C_SESSION__I2C_SESSION_H_

#include <base/signal.h>
#include <session/session.h>

namespace I2C {

	struct Session : Genode::Session
	{
		static const char *service_name() { return "I2C"; }

		virtual ~Session() { }

		/**
		 * Read a single byte from the device
		 *
		 * \param address the address of the device on the bus
		 * \param register the register to read
		 * \param out the destination to put data
		 *
		 */
		virtual bool read_byte(Genode::uint8_t address,
			Genode::uint8_t register, Genode::uint8_t *out) = 0;
		
		/**
		 * Write a single byte to the device
		 *
		 * \param address the address of the device on the bus
		 * \param register the register to read
		 * \param in the value to write
		 *
		 */
		virtual bool write_byte(Genode::uint8_t address,
			Genode::uint8_t register, Genode::uint8_t in) = 0;


		/*********************
		 ** RPC declaration **
		 *********************/

		GENODE_RPC(Rpc_read_byte, bool, read_byte,
			Genode::uint8_t, Genode::uint8_t, Genode::uint8_t*);
		GENODE_RPC(Rpc_write_byte, bool, write_byte,
			Genode::uint8_t, Genode::uint8_t, Genode::uint8_t);

		GENODE_RPC_INTERFACE(
			Rpc_read_byte,
			Rpc_write_byte
		);
	};
}

#endif /* _INCLUDE__I2C_SESSION__I2C_SESSION_H_ */
