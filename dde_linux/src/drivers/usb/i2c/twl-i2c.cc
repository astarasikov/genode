/*
 * \brief TI TWL stub routines
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

#include <i2c_session/connection.h>
#include <timer_session/connection.h>

#include <base/printf.h>
#include <lx_emul.h>

extern "C" {
#include <linux/i2c/twl.h>
}

static I2C::Connection *i2c;

/* mapping the module id to slave id and base address */
struct twl_mapping {
	unsigned char sid;	/* Slave ID */
	unsigned char base;	/* base address */
};

#define SUB_CHIP_ID0 0
#define SUB_CHIP_ID1 1
#define SUB_CHIP_ID2 2
#define SUB_CHIP_ID3 3

/* subchip/slave 0 0x48 - POWER */
#define TWL6030_BASEADD_RTC		0x0000
#define TWL6030_BASEADD_MEM		0x0017
#define TWL6030_BASEADD_PM_MASTER	0x001F
#define TWL6030_BASEADD_PM_SLAVE_MISC	0x0030 /* PM_RECEIVER */
#define TWL6030_BASEADD_PM_MISC		0x00E2
#define TWL6030_BASEADD_PM_PUPD		0x00F0

/* subchip/slave 1 0x49 - FEATURE */
#define TWL6030_BASEADD_USB		0x0000
#define TWL6030_BASEADD_GPADC_CTRL	0x002E
#define TWL6030_BASEADD_AUX		0x0090
#define TWL6030_BASEADD_PWM		0x00BA
#define TWL6030_BASEADD_GASGAUGE	0x00C0
#define TWL6030_BASEADD_PIH		0x00D0
#define TWL6030_BASEADD_CHARGER		0x00E0
#define TWL6025_BASEADD_CHARGER		0x00DA

/* subchip/slave 2 0x4A - DFT */
#define TWL6030_BASEADD_DIEID		0x00C0

/* subchip/slave 3 0x4B - AUDIO */
#define TWL6030_BASEADD_AUDIO		0x0000
#define TWL6030_BASEADD_RSV		0x0000
#define TWL6030_BASEADD_ZERO		0x0000

static struct twl_mapping twl6030_map[] = {
	/*
	 * NOTE:  don't change this table without updating the
	 * <linux/i2c/twl.h> defines for TWL4030_MODULE_*
	 * so they continue to match the order in this table.
	 */
	{ SUB_CHIP_ID1, TWL6030_BASEADD_USB },
	{ SUB_CHIP_ID3, TWL6030_BASEADD_AUDIO },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_DIEID },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID1, TWL6030_BASEADD_PIH },

	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID1, TWL6030_BASEADD_GPADC_CTRL },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },

	{ SUB_CHIP_ID1, TWL6030_BASEADD_CHARGER },
	{ SUB_CHIP_ID1, TWL6030_BASEADD_GASGAUGE },
	{ SUB_CHIP_ID1, TWL6030_BASEADD_PWM },
	{ SUB_CHIP_ID0, TWL6030_BASEADD_ZERO },
	{ SUB_CHIP_ID1, TWL6030_BASEADD_ZERO },

	{ SUB_CHIP_ID2, TWL6030_BASEADD_ZERO },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_ZERO },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID2, TWL6030_BASEADD_RSV },
	{ SUB_CHIP_ID0, TWL6030_BASEADD_PM_MASTER },
	{ SUB_CHIP_ID0, TWL6030_BASEADD_PM_SLAVE_MISC },

	{ SUB_CHIP_ID0, TWL6030_BASEADD_RTC },
	{ SUB_CHIP_ID0, TWL6030_BASEADD_MEM },
	{ SUB_CHIP_ID1, TWL6025_BASEADD_CHARGER },
};

extern "C" {

int twl_i2c_write_u8(u8 mod_no, u8 val, u8 reg) {
	u8 offset = twl6030_map[mod_no].base;

	PDBG("%s: offset=%x val=%x reg=%x", __func__, offset, val, reg);
	i2c->write_byte(0x48, offset + reg, val);
	return 0;
}

int twl_i2c_read_u8(u8 mod_no, u8 *val, u8 reg) {
	u8 offset = twl6030_map[mod_no].base;
	
	i2c->read_byte(0x48, offset + reg, val);

	PDBG("%s: offset=%x, *val=%x, reg=%x", __func__, offset, *val, reg);
	
	return 0;
}

int twl6030_interrupt_unmask(u8 bit_mask, u8 offset) {
	PDBG("%s: bit_mask=%x offset=%x", __func__, bit_mask, offset);
	return 0;
}

int twl6030_interrupt_mask(u8 bit_mask, u8 offset) {
	PDBG("%s: bit_mask=%x offset=%x", __func__, bit_mask, offset);
	return 0;
}

} //extern "C"

void platform_i2c_init(Services *services) {
	PDBG("+%s\n", __func__);

	static I2C::Connection _i2c;
	i2c = &_i2c;

	static Timer::Connection timer;
}
