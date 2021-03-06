/*
 * \brief  PCI session capability type
 * \author Norman Feske
 * \date   2008-08-16
 */

/*
 * Copyright (C) 2008-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__PCI_SESSION__CAPABILITY_H_
#define _INCLUDE__PCI_SESSION__CAPABILITY_H_

#include <base/capability.h>
#include <pci_session/pci_session.h>

namespace Pci { typedef Genode::Capability<Session> Session_capability; }

#endif /* _INCLUDE__PCI_SESSION__CAPABILITY_H_ */
