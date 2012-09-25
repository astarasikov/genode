/*
 * \brief  OMAP4 USB OTG and client initialization
 * \author Alexander Tarasikov
 * \date   2012-09-19
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <platform/platform.h>

#include <io_mem_session/connection.h>
#include <util/mmio.h>

#include <base/printf.h>
#include <lx_emul.h>

#include <linux/i2c/twl.h>
#include <linux/usb/musb.h>

using namespace Genode;

#define MUSB_MODE MUSB_PERIPHERAL
//#define MUSB_MODE MUSB_OTG
#define MUSB_POWER 100

static struct omap_musb_board_data musb_board_data = {};
static struct musb_hdrc_config musb_config = {};
static struct musb_hdrc_platform_data musb_plat = {};

extern "C" int module_twl6030_usb_init(void);
extern "C" int module_omap2430_init(void);
extern "C" int module_musb_init(void);
extern "C" int module_gadget_ether_init(void);
extern "C" int module_usb_udc_init(void);
extern "C" int module_gadget_zero_init(void);

#define OMAP44XX_L4_CORE_BASE 0x4a000000
#define OMAP2_OTG_BASE			(OMAP44XX_L4_CORE_BASE + 0xab000)
#define INT_24XX_USB_IRQ_OTG	124

#define SCM_BASE 0x4a002000

struct Phy : Genode::Mmio {
	Phy(addr_t const mmio_base) : Mmio(mmio_base) {}
	
	struct DevConf : Register<0x300, 32> {
		enum {
			PowerDown = 1,
			PowerUp = ~1,
		};
	};
	struct Control : Register<0x33c, 32> {
		struct AValid : Bitfield<0, 1> {};
		struct BValid : Bitfield<1, 1> {};
		struct VBusValid : Bitfield<2, 1> {};
		struct SessionEnd : Bitfield<3, 1> {};
		struct IdDig : Bitfield<4, 1> {};
	};

	void init() {
		write<Phy::DevConf>(Phy::DevConf::PowerUp);
	}

	void power(int id, int on) {
		write<Phy::Control::SessionEnd>(!on);
		write<Phy::Control::IdDig>(!on || id);

		write<Phy::Control::AValid>(on);
		write<Phy::Control::VBusValid>(on);
	}

	void suspend(int suspend) {
		if (suspend) {
			write<Phy::DevConf>(Phy::DevConf::PowerDown);
		}
		else {
			write<Phy::DevConf>(Phy::DevConf::PowerUp);
		}
	}
};
static Phy *phy;

static int phy_init(struct device *dev) {
	PDBG("+%s()", __func__);
	phy->init();
	return 0;
}

static int phy_exit(struct device *dev) {
	PDBG("+%s()", __func__);
	return 0;
}

static int phy_power(struct device *dev, int id, int on) {
	PDBG("+%s(id=%d on=%d)", __func__, id, on);
	phy->power(id, on);
	return 0;
}

static int phy_set_clock(struct device *dev, int on) {
	PDBG("+%s(on=%d)", __func__, on);
	return 0;
}

static int phy_suspend(struct device *dev, int suspend) {
	PDBG("+%s(suspend=%d)", __func__, suspend);
	phy->suspend(suspend);
	return 0;
}

static struct resource otg_resources[2] = {};

static struct twl4030_usb_data twl_data = {}; 

static void init_twl6030_usb(void) {
	module_twl6030_usb_init();
	platform_device *pdev = NULL;
	
	pdev = (platform_device*)kzalloc(sizeof(platform_device), 0);
	if (!pdev) {
		PERR("%s: no memory for the device", __func__);
		return;
	}

	pdev->name = "twl6030_usb";
	pdev->id = -1;

	twl_data.phy_init = phy_init;
	twl_data.phy_exit = phy_exit;
	twl_data.phy_power = phy_power;
	twl_data.phy_set_clock = phy_set_clock;
	twl_data.phy_suspend = phy_suspend;

	pdev->dev.platform_data = &twl_data;
	platform_device_register(pdev);
}

void panda_otg_init(Services *services) {
	platform_device *pdev = NULL;
	PINF("%s: initializing otg modules", __func__);

	static Io_mem_connection io_scm(SCM_BASE, 0x1000);
	addr_t scm_base = (addr_t)env()->rm_session()->attach(io_scm.dataspace());
	static Phy _phy(scm_base);
	phy = &_phy;

	init_twl6030_usb();
	module_musb_init();
	module_omap2430_init();

	pdev = (platform_device*)kzalloc(sizeof(platform_device), 0);
	if (!pdev) {
		PERR("%s: no memory for the device", __func__);
		return;
	}

	otg_resources[0].start = OMAP2_OTG_BASE;
	otg_resources[0].end = OMAP2_OTG_BASE + 0xff;
	otg_resources[0].flags = IORESOURCE_MEM;

	otg_resources[1].start = INT_24XX_USB_IRQ_OTG;
	otg_resources[1].flags = IORESOURCE_IRQ;
	otg_resources[1].name = "mc";

	musb_board_data.interface_type = MUSB_INTERFACE_UTMI;
	//musb_board_data.interface_type = MUSB_INTERFACE_ULPI;
	musb_board_data.mode = MUSB_MODE;
	musb_board_data.power = MUSB_POWER;

	musb_config.multipoint = 1;
	musb_config.dyn_fifo = 1;
	musb_config.num_eps = 16;
	musb_config.ram_bits = 12;

	musb_plat.mode = MUSB_MODE;
	musb_plat.config = &musb_config;
	musb_plat.power = MUSB_POWER >> 1;
	musb_plat.board_data = &musb_board_data;

	pdev->name = "musb-omap2430";
	pdev->id = -1;
	pdev->num_resources = ARRAY_SIZE(otg_resources);
	pdev->resource = otg_resources;
	pdev->dev.platform_data = &musb_plat;

	static u64 musb_dmamask = DMA_BIT_MASK(32);
	pdev->dev.dma_mask = &musb_dmamask;
	pdev->dev.coherent_dma_mask = musb_dmamask;

	PINF("%s: reigstering musb", __func__);
	platform_device_register(pdev);
	
	//module_gadget_ether_init();
	module_gadget_zero_init();
}
