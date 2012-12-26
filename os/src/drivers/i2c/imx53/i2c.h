/*
 * \brief  I2C Driver for i.MX53
 * \author Ivan Loskutov
 * \author Alexander Tarasikov
 * \author Nikolay Golikov
 * \date   2012-11-26
 */

/*
 * Copyright (C) 2012 Ksys Labs LLC
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <util/mmio.h>

namespace I2C {

	using namespace Genode;
	class Imx53_I2C;
}

struct I2C::Imx53_I2C : Mmio
{
	enum { I2C_TIMEOUT   = 100 };

	struct Iadr : Register<0x0, 8> { }; /* address register */
	struct Ifdr : Register<0x4, 8> { }; /* frequency divider register */

	/* control register */
	struct I2cr : Register<0x8, 8>
	{
		struct Rsta : Bitfield<2, 1> { };
		struct Txnoak : Bitfield<3, 1> { };
		struct Mtx  : Bitfield<4, 1> { };
		struct Msta : Bitfield<5, 1> { };
		struct Ien  : Bitfield<7, 1> { };
	};

	/* status register */
	struct I2sr : Register<0xc, 8>
	{
		struct Rxak : Bitfield<0, 1> { };
		struct Iff  : Bitfield<1, 1> { };
		struct Ial  : Bitfield<4, 1> { };
		struct Ibb  : Bitfield<5, 1> { };
		struct Icf  : Bitfield<7, 1> { };
	};

	struct I2dr : Register<0x10, 16> { }; /* data I/O register */

	Timer::Connection _timer;

	Imx53_I2C(Genode::addr_t const mmio_base) : Mmio(mmio_base)
	{
		/* Store divider value */
		write<Ifdr>(16);

		/* Rest module */
		write<I2cr>(0);
		write<I2sr>(0);
		write<Iadr>(1<<1);
	}

	~Imx53_I2C() {
	}

	template <class REG>
	bool wait_for_reg(uint8_t value)
	{
		int timeout = I2C_TIMEOUT;

		while ((read<REG>() != value) && (timeout--) > 0) {
			if (read<I2sr::Ial>()) {
				PERR("Arbitration lost I2sr:%x I2cr%x\n", read<I2sr>(), read<I2cr>());
				return false;
			}
			_timer.msleep(1);
		}

		if (timeout <= 0) {
			PERR("timed out in wait_for_reg: I2sr=0x%x, reg=0x%x, value=0x%x\n",
			     read<I2sr>(), read<REG>(), value);
			return false;
		}

		return true;
	}

	bool tx_byte(uint8_t byte)
	{
		write<I2sr>(0);
		write<I2dr>(byte);

		if (!wait_for_reg<I2sr::Iff>(1)) {
			return false;
		} else if (read<I2sr::Rxak>()) {
			PERR("tx_byte failed");
			return false;
		}

		return true;
	}

	bool init_tranfer(uint8_t chip, uint8_t addr)
	{
		/* Enable I2C */
		if (!read<I2cr::Ien>()) {
			write<I2cr::Ien>(1);

			/* Wait for controller to be stable */
			_timer.msleep(1);
		}
		if (!wait_for_reg<I2sr::Ibb>(0)) {
			PERR("Wait for bus idle timeout\n");
			return false;
		}

		/* Clear status register */
		write<I2sr>(1);

		/* Set master mode */
		write<I2cr::Msta>(1);
		if (!wait_for_reg<I2sr::Ibb>(1)) {
			PERR("Wait for bus busy timeout\n");
			return false;
		}

		write<I2cr::Mtx>(1);
		write<I2cr::Txnoak>(1);

		if (!tx_byte(chip << 1)) {
			PERR("Tranfer chip address failed\n");
			return false;
		}

		return tx_byte(addr & 0xff);
	}

	void stop(void)
	{
		write<I2cr::Msta>(0);
		write<I2cr::Mtx>(0);

		if (!wait_for_reg<I2sr::Ibb>(0)) {
			PERR("Stop failed!\n");
		}
	}

	int i2c_read(uint8_t chip, uint8_t addr, uint8_t *buffer, uint8_t len)
	{
		if (!init_tranfer(chip, addr)) {
			stop();
			return -1;
		}

		/* Repeat start */
		write<I2cr::Rsta>(1);

		if (!(tx_byte((chip << 1) | 1))) {
			stop();
			return -1;
		}

		/* Setup bus to read data */
		write<I2cr::Mtx>(0);
		if (len != 1) {
			write<I2cr::Txnoak>(0);
		}

		write<I2sr>(0);

		/* Dummy read for clear ICF */
		read<I2dr>();

		/* Read data */
		for (uint8_t i = 0; i < len; i++) {
			if (!wait_for_reg<I2sr::Iff>(1)) {
				PERR("Read timeout");
				stop();
				return -1;
			}

			/**
			 * It must generate STOP before read I2DR to prevent
			 * controller from generating another clock cycle
			 */
			if (i == (len - 1)) {
				stop();
			} else if (i == (len - 2)) {
				write<I2cr::Txnoak>(1);
			}

			write<I2sr>(0);
			buffer[i] = read<I2dr>();
		}

		return 0;
	}

	int i2c_write(uint8_t chip, uint8_t addr, uint8_t *buffer, uint8_t len)
	{
		if (!init_tranfer(chip, addr)) {
			stop();
			return -1;
		}

		for (uint8_t i = 0; i < len; i++) {
			if (!tx_byte(buffer[i])) {
				PERR("Write timeout");
				stop();
				return -1;
			}
		}

		stop();

		return 0;
	}

};

#endif /* _I2C_H_ */
