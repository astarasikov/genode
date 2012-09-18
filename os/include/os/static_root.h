/*
 * \brief  Common static session root implementation
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

#ifndef _INCLUDE__OS__STATIC_ROOT_H_
#define _INCLUDE__OS__STATIC_ROOT_H_

/**
 * Root interface that hands out a statically created session
 */
template <typename SESSION_TYPE>
class Static_root : public Genode::Rpc_object<Genode::Typed_root<SESSION_TYPE> >
{
	private:

		typedef Genode::Capability<SESSION_TYPE> Session_capability;

		Session_capability _session;

	public:

		/**
		 * Constructor
		 *
		 * \param session  session to be provided to the client
		 */
		Static_root(Session_capability session) : _session(session) { }


		/********************
		 ** Root interface **
		 ********************/

		Genode::Session_capability session(
			Genode::Root::Session_args const &args) { return _session; }
		void upgrade(Genode::Session_capability,
			Genode::Root::Upgrade_args const &) { }
		void close(Genode::Session_capability) { }
};

#endif //_INCLUDE__OS__STATIC_ROOT_H_
