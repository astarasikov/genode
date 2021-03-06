/*
 * \brief  LwIP Genode ethernet interface
 * \author Stefan Kalkowski
 * \date   2009-11-05
 */

/*
 * Copyright (C) 2009-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _LWIP__NIC_H_
#define _LWIP__NIC_H_

/**
 * Initializes the genode nic backend.
 *
 * \param netif pointer to struct, where nic related stuff is registered.
 */
err_t genode_netif_init(struct netif *netif);

#endif //_LWIP__NIC_H_
