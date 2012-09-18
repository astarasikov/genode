#include <platform/platform.h>

#include <base/printf.h>
#include <lx_emul.h>
#include <linux/usb/musb.h>

using namespace Genode;

#define MUSB_MODE MUSB_OTG
#define MUSB_POWER 100

static struct omap_musb_board_data musb_board_data = {};
static struct musb_hdrc_config musb_config = {};
static struct musb_hdrc_platform_data musb_plat = {};

extern "C" int module_omap2430_init(void);
extern "C" int module_musb_init(void);

void panda_otg_init(Services *services) {
	platform_device *pdev = NULL;
	PINF("%s: initializing otg modules", __func__);

	module_musb_init();
	//module_omap2430_init();

	pdev = (platform_device*)kzalloc(sizeof(platform_device), 0);
	if (!pdev) {
		PERR("%s: no memory for the device", __func__);
		return;
	}

	musb_board_data.interface_type = MUSB_INTERFACE_ULPI;
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

	pdev->name = "musb-hdrc";
	pdev->id = -1;
	pdev->num_resources = 0;
	pdev->resource = 0;
	pdev->dev.platform_data = &musb_plat;

	static u64 musb_dmamask = DMA_BIT_MASK(32);
	pdev->dev.dma_mask = &musb_dmamask;
	pdev->dev.coherent_dma_mask = musb_dmamask;

	PINF("%s: reigstering musb", __func__);
	platform_device_register(pdev);
}
