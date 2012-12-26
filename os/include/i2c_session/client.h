/*
 * \brief  Client-side i2c interface
 * \author Alexander Tarasikov <tarasikov@ksyslabs.org>
 * \author Nikolay Golikov <nik@ksyslabs.org>
 * \date   2012-11-30
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
#include <dataspace/capability.h>
#include <base/rpc_client.h>
#include <base/env.h>
#include <base/printf.h>


namespace I2C {

	class Session_client : public Genode::Rpc_client<Session>
	{
		private:
			struct Io_buffer
			{
				Genode::Dataspace_capability ds_cap;
				char						*base;
				Genode::size_t			   size;

				Io_buffer(Genode::Dataspace_capability ds_cap)
				:
					ds_cap(ds_cap),
					base(Genode::env()->rm_session()->attach(ds_cap)),
					size(ds_cap.call<Genode::Dataspace::Rpc_size>())
				{ }

				~Io_buffer()
				{
					Genode::env()->rm_session()->detach(base);
				}
			};

			Io_buffer _io_buffer;

		public:
			explicit Session_client(Session_capability session)
			:
				Genode::Rpc_client<Session>(session),
				_io_buffer(call<Rpc_dataspace>())
			{ }

			bool read(Genode::uint8_t address, Genode::uint8_t reg,
			          Genode::uint8_t *out, Genode::size_t len)
			{
				if (len > _io_buffer.size) {
					PERR("len (%d) exceeds the buffrer size (%d)", 
					             len, _io_buffer.size);
					return false;
				}

				bool res = call<Rpc_read>(address, reg, out, len);
				Genode::memcpy(out, _io_buffer.base, len);

				return res;
			}

			bool write(Genode::uint8_t address, Genode::uint8_t reg,
			           Genode::uint8_t *in, Genode::size_t len)
			{
				if (len > _io_buffer.size) {
					PERR("len (%d) exceeds the buffrer size (%d)", 
					             len, _io_buffer.size);
					return false;
				}

				Genode::memcpy(_io_buffer.base, in, len);
				return call<Rpc_write>(address, reg, in, len);
			}

	};
}

#endif /* _INCLUDE__I2C_SESSION__CLIENT_H_ */
