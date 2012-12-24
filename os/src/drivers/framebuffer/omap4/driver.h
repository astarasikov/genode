/*
 * \brief  Frame-buffer driver for the OMAP4430 display-subsystem (HDMI)
 * \author Norman Feske
 * \author Martin Stein
 * \date   2012-06-01
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <drivers/board.h>
#include <os/attached_io_mem_dataspace.h>
#include <timer_session/connection.h>
#include <util/mmio.h>

/* local includes */
#include <dss.h>
#include <dispc.h>
#include <hdmi.h>

#define ON_TUNA 1


namespace Framebuffer {
	using namespace Genode;
	class Driver;
}


class Framebuffer::Driver
{
	public:

		enum Mode { MODE_1024_768 };
		enum Format { FORMAT_RGB565 };

	private:

		struct Timer_delayer : Timer::Connection, Mmio::Delayer
		{
			/**
			 * Implementation of 'Delayer' interface
			 */
			void usleep(unsigned us)
			{
				/* polling */
				if (us == 0)
					return;

				unsigned ms = us / 1000;
				if (ms == 0)
					ms = 1;

				Timer::Connection::msleep(ms);
			}
		} _delayer;

		/* display sub system registers */
		Attached_io_mem_dataspace _dss_mmio;
		Dss                       _dss;

		/* display controller registers */
		Attached_io_mem_dataspace _dispc_mmio;
		Dispc                     _dispc;

		/* HDMI controller registers */
		Attached_io_mem_dataspace _hdmi_mmio;
		Hdmi                      _hdmi;

	public:

		Driver();

		static size_t bytes_per_pixel(Format format)
		{
			switch (format) {
			case FORMAT_RGB565: return 2;
			}
			return 0;
		}

		static size_t width(Mode mode)
		{
		#if ON_TUNA
			return 720;
		#endif
			switch (mode) {
			case MODE_1024_768: return 1024;
			}
			return 0;
		}

		static size_t height(Mode mode)
		{
		#if ON_TUNA
			return 1280;
		#endif
			switch (mode) {
			case MODE_1024_768: return 768;
			}
			return 0;
		}

		static size_t buffer_size(Mode mode, Format format)
		{
			return bytes_per_pixel(format)*width(mode)*height(mode);
		}

		bool init(Mode mode, Format format, addr_t phys_base);
};


Framebuffer::Driver::Driver()
:
	_dss_mmio(Board::DSS_MMIO_BASE, Board::DSS_MMIO_SIZE),
	_dss((addr_t)_dss_mmio.local_addr<void>()),

	_dispc_mmio(Board::DISPC_MMIO_BASE, Board::DISPC_MMIO_SIZE),
	_dispc((addr_t)_dispc_mmio.local_addr<void>()),

	_hdmi_mmio(Board::HDMI_MMIO_BASE, Board::HDMI_MMIO_SIZE),
	_hdmi((addr_t)_hdmi_mmio.local_addr<void>())
{ }


bool Framebuffer::Driver::init(Framebuffer::Driver::Mode   mode,
                               Framebuffer::Driver::Format format,
                               Framebuffer::addr_t         phys_base)
{
	/* enable display core clock and set divider to 1 */
	#if 0
	_dispc.write<Dispc::Divisor::Lcd>(2);
	_dispc.write<Dispc::Divisor::Enable>(1);
	#endif
	
	_dispc.write<Dispc::Control1::Lcd_enable>(0);

	/* set load mode */
	_dispc.write<Dispc::Config1::Load_mode>(Dispc::Config1::Load_mode::DATA_EVERY_FRAME);

	Dispc::Gfx_attributes::access_t pixel_format = 0;
	switch (format) {
	case FORMAT_RGB565: pixel_format = Dispc::Gfx_attributes::Format::RGB16; break;
	}
	_dispc.write<Dispc::Gfx_attributes::Format>(pixel_format);
	
	//_dispc.write<Dispc::Size_tv::Width>(width(mode) - 1);
	//_dispc.write<Dispc::Size_tv::Height>(height(mode) - 1);

	_dispc.write<Dispc::Gfx_ba0>(phys_base);
	_dispc.write<Dispc::Gfx_ba1>(phys_base);

	//_dispc.write<Dispc::Gfx_size::Sizex>(width(mode) - 1);
	//_dispc.write<Dispc::Gfx_size::Sizey>(height(mode) - 1);

	_dispc.write<Dispc::Global_buffer>(0x6d2240);
	_dispc.write<Dispc::Gfx_attributes::Enable>(1);

	#if 0
	_dispc.write<Dispc::Gfx_attributes::Channelout>(Dispc::Gfx_attributes::Channelout::TV);
	_dispc.write<Dispc::Gfx_attributes::Channelout2>(Dispc::Gfx_attributes::Channelout2::PRIMARY_LCD);
	#endif

	#if 1
	_dispc.write<Dispc::Control1::Lcd_enable>(1);
	_dispc.write<Dispc::Control1::Go_lcd>(1);
	#endif

	return true;
}
