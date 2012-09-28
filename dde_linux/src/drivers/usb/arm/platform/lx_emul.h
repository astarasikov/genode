/*
 * \brief  Platform specific part of the Linux API emulation
 * \author Sebastian Sumpf
 * \date   2012-06-18
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */
#ifndef _ARM__PLATFORM__LX_EMUL_H_
#define _ARM__PLATFORM__LX_EMUL_H_

/*************************
 ** asm-generic/sizes.h **
 *************************/

enum {
	SZ_1K = 0x00000400,
	SZ_4K = 0x00001000a,
};

struct platform_device
{
	const char      *name;
	int              id;
	struct device    dev;
	u32              num_resources;
	struct resource *resource;
	const struct mfd_cell *mfd_cell;
};


/**********************
 ** linux/usb/ulpi.h **
 **********************/

enum {
	ULPI_FUNC_CTRL_RESET = (1 << 5),
	ULPI_FUNC_CTRL       = (1 << 2),
};

/*
 * Macros for Set and Clear
 * See ULPI 1.1 specification to find the registers with Set and Clear offsets
 */

#define ULPI_SET(a) (a + 1)

/*******************************************
 ** arch/arm/plat-omap/include/plat/usb.h **
 *******************************************/

enum { OMAP3_HS_USB_PORTS = 2 };

enum usbhs_omap_port_mode
{
	OMAP_EHCI_PORT_MODE_NONE,
	OMAP_EHCI_PORT_MODE_PHY,
};


struct ehci_hcd_omap_platform_data
{
	enum usbhs_omap_port_mode  port_mode[OMAP3_HS_USB_PORTS];
	struct regulator          *regulator[OMAP3_HS_USB_PORTS];
};

struct regulator;

struct omap_musb_board_data {
	u8	interface_type;
	u8	mode;
	u16	power;
	unsigned extvbus:1;
	void	(*set_phy_power)(u8 on);
	void	(*clear_irq)(void);
	void	(*set_mode)(u8 mode);
	void	(*reset)(void);
};

enum musb_interface    {MUSB_INTERFACE_ULPI, MUSB_INTERFACE_UTMI};

/*****************************
 ** linux/platform_device.h **
 *****************************/

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
};

struct resource *platform_get_resource_byname(struct platform_device *, unsigned int, const char *);
int platform_get_irq_byname(struct platform_device *, const char *);

int platform_driver_register(struct platform_driver *);
int platform_device_register(struct platform_device *);
extern struct resource *platform_get_resource(struct platform_device *, unsigned int, unsigned int);
extern int platform_driver_probe(struct platform_driver *driver,
		int (*probe)(struct platform_device *));

extern void platform_device_del(struct platform_device *pdev);
extern void platform_device_put(struct platform_device *pdev);
extern void platform_set_drvdata(struct platform_device *pdev, void *data);
extern struct platform_device *platform_device_alloc(const char *name, int id);
extern int platform_device_add_data(struct platform_device *pdev,
	const void *data, size_t size);
extern int platform_device_add_resources(struct platform_device *pdev,
					 const struct resource *res,
					 unsigned int num);
/**********************
 ** asm/generic/io.h **
 **********************/

static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	return *(const volatile u32 __force *) addr;
}

static inline void __raw_writel(u32 b, volatile void __iomem *addr)
{
	*(volatile u32 __force *) addr = b;
}


static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	return *(const volatile u8 __force *) addr;
}

static inline void __raw_writeb(u8 b, volatile void __iomem *addr)
{
	*(volatile u8 __force *) addr = b;
}

static inline u16 __raw_readw(const volatile void __iomem *addr) {
	return *(const volatile u16 __force *) addr;
}

static inline void __raw_writew(u16 b, volatile void __iomem *addr) {
	*(volatile u16 __force*) addr = b;
}

static inline void writesw(void __iomem *dst, const u8 *src, u16 len) {
	memcpy(dst, src, len);
}

#define writesb writesw
#define writesl writesw

static inline void readsw(void __iomem *src, u8 *dst, u16 len) {
	memcpy(dst, src, len);
}

#define readsb readsw
#define readsl readsw

/********************************
 ** linux/regulator/consumer.h **
 ********************************/

int    regulator_enable(struct regulator *);
int    regulator_disable(struct regulator *);
void   regulator_put(struct regulator *regulator);
struct regulator *regulator_get(struct device *dev, const char *id);


/*******************************************
 ** arch/arm/plat-omap/include/plat/usb.h **
 *******************************************/

int omap_usbhs_enable(struct device *dev);
void omap_usbhs_disable(struct device *dev);

#endif /* _ARM__PLATFORM__LX_EMUL_H_ */
