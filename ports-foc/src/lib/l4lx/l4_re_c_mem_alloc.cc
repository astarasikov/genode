/*
 * \brief  L4Re functions needed by L4Linux.
 * \author Stefan Kalkowski
 * \date   2011-04-17
 */

/*
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/printf.h>
#include <base/capability.h>

/* L4lx includes */
#include <dataspace.h>
#include <env.h>

namespace Fiasco {
#include <l4/re/c/mem_alloc.h>
}

using namespace Fiasco;

static const bool DEBUG = false;

extern "C" {

	long l4re_ma_alloc(unsigned long size, l4re_ds_t const mem,
	                   unsigned long flags)
	{
		using namespace L4lx;

		if (DEBUG)
			PDBG("size=%lx mem=%lx flags=%lx", size, mem, flags);

		Genode::Dataspace_capability cap = Genode::env()->ram_session()->alloc(size);
		Dataspace *ds = new (Genode::env()->heap())
			Dataspace("lx_memory", size, cap, mem);
		Env::env()->dataspaces()->insert(ds);
		return 0;
	}


	long l4re_ma_free(l4re_ds_t const mem)
	{
		PWRN("%s: Not implemented yet!",__func__);
		return 0;
	}

}
