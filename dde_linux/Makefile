#
# \brief  Download integrate Linux kernel sources with Genode
# \author Norman Feske
# \date   2012-01-28

CONTRIB_DIR  = contrib
DOWNLOAD_DIR = download
VERBOSE     ?= @
ECHO         = @echo
PATCHES      := $(shell find patches -name \*.patch)

LINUX      = linux-3.2.2
LINUX_TBZ2 = $(LINUX).tar.bz2
LINUX_URL  = http://www.kernel.org/pub/linux/kernel/v3.x/$(LINUX_TBZ2)

# Linux utilities
CONTENT += include/linux/list.h
CONTENT += $(addprefix include/linux/,pci_ids.h pci_regs.h usb.h hid.h hiddev.h input.h mod_devicetable.h)
CONTENT += $(addprefix include/linux/byteorder/,generic.h little_endian.h)
CONTENT += include/linux/swab.h
CONTENT += $(addprefix include/asm-generic/bitops/,__ffs.h non-atomic.h)

# USB core
CONTENT += drivers/usb/core
CONTENT += drivers/usb/usb-common.c
CONTENT_INCLUDE_USB := ch11.h ch9.h ehci_def.h hcd.h input.h otg.h quirks.h storage.h
CONTENT += $(addprefix include/linux/usb/,$(CONTENT_INCLUDE_USB))

# needed by usb/core/devio.c
CONTENT += include/linux/usbdevice_fs.h include/asm-generic/ioctl.h

# USB host-controller driver
CONTENT_USB_HOST := ehci.h ehci-hcd.c ehci-hub.c ehci-dbg.c ehci-mem.c \
                    ehci-omap.c ehci-q.c ehci-pci.c ehci-sched.c ehci-sysfs.c 
CONTENT_USB_HOST += ohci.h ohci-hcd.c ohci-hub.c ohci-dbg.c ohci-mem.c \
                    ohci-q.c ohci-pci.c ehci-lpm.c
CONTENT_USB_HOST += uhci-hcd.h uhci-hcd.c uhci-debug.c uhci-q.c uhci-hub.c \
                    uhci-pci.c
CONTENT_USB_HOST += pci-quirks.h pci-quirks.c xhci-ext-caps.h
CONTENT += $(addprefix drivers/usb/host/,$(CONTENT_USB_HOST))

# USB storage driver
CONTENT += drivers/usb/storage/
CONTENT += include/linux/usb_usual.h

# SCSI support for storage
CONTENT += $(addprefix drivers/scsi/,scsi.h scsi.c constants.c scsi_priv.h scsi_logging.h)
CONTENT += $(addprefix include/scsi/,scsi.h scsi_host.h)

# USB hid driver
CONTENT += drivers/hid/hid-input.c drivers/hid/hid-core.c drivers/hid/hid-ids.h
CONTENT += drivers/hid/usbhid

# needed by USB hid
CONTENT_INPUT := input.c evdev.c input-compat.h
CONTENT += $(addprefix drivers/input/,$(CONTENT_INPUT))
CONTENT += include/linux/input/mt.h

# USB OTG and UDC (usb device client) drivers
CONTENT_USB_GADGET := gadget_chips.h f_loopback.c usbstring.c composite.c \
	epautoconf.c f_subset.c config.c zero.c g_zero.h f_ecm.c \
	u_ether.c ether.c udc-core.c u_ether.h f_eem.c f_sourcesink.c
CONTENT_INCLUDE_USB_GADGET := gadget.h usbnet.h composite.h
CONTENT += $(addprefix drivers/usb/gadget/,$(CONTENT_USB_GADGET))
CONTENT += $(addprefix include/linux/usb/,$(CONTENT_INCLUDE_USB_GADGET))

CONTENT_USB_OTG := otg.c
CONTENT += $(addprefix drivers/usb/otg/,$(CONTENT_USB_OTG))
CONTENT += $(addprefix include/linux/usb/,otg.h)

# Mentor Graphics USB driver
CONTENT_USB_MUSB := musb_dma.h musb_gadget.c musb_host.h musb_gadget.h \
	musb_host.c musb_core.c musb_debug.h musb_core.h musb_virthub.c \
	musbhsdma.c musb_regs.h musb_gadget_ep0.c musb_io.h musb_debugfs.c \
	musbhsdma.h
CONTENT += $(addprefix drivers/usb/musb/,$(CONTENT_USB_MUSB)) \
	include/linux/usb/musb.h

CONTENT_USB_OMAP4 := drivers/usb/otg/twl6030-usb.c \
	drivers/usb/musb/omap2430.c drivers/usb/musb/omap2430.h \
	include/linux/mfd/twl6040.h include/linux/mfd/core.h \
	include/linux/i2c/twl.h
CONTENT += $(CONTENT_USB_OMAP4)

# Panda board usb network driver
CONTENT_NET = usbnet.c smsc95xx.c smsc95xx.h
CONTENT    += $(addprefix drivers/net/usb/,$(CONTENT_NET))
CONTENT    += include/linux/usb/usbnet.h

#
#
# Utility to check if a tool is installed
#
check_tool = $(if $(shell which $(1)),,$(error Need to have '$(1)' installed.))

$(call check_tool,wget)
$(call check_tool,patch)

#
# Print help information by default
#
help:
	$(ECHO)
	$(ECHO) "Download integrate Linux kernel sources with Genode"
	$(ECHO)
	$(ECHO) "--- available commands ---"
	$(ECHO) "prepare  - download and integrate Linux source code"
	$(ECHO) "clean    - remove contib sources except downloaded archives"
	$(ECHO) "cleanall - remove contib sources and downloaded archives"
	$(ECHO)

prepare: $(CONTRIB_DIR)/.prepared

$(CONTRIB_DIR)/.prepared: Makefile
$(CONTRIB_DIR)/.prepared: $(DOWNLOAD_DIR)/$(LINUX_TBZ2)
	$(ECHO) "extracting source code to '$(CONTRIB_DIR)'"
	$(VERBOSE)tar xfj $< --transform "s/$(LINUX)/$(CONTRIB_DIR)/" $(addprefix $(LINUX)/,$(CONTENT))
	$(VERBOSE)touch $@
	$(ECHO) "applying patches to '$(CONTRIB_DIR)/'"
	$(VERBOSE)for i in $(PATCHES); do patch -d $(CONTRIB_DIR) -p1 < $$i; done


$(DOWNLOAD_DIR):
	$(VERBOSE)mkdir -p $@

$(DOWNLOAD_DIR)/$(LINUX_TBZ2): $(DOWNLOAD_DIR)
	$(ECHO) "downloading source code to '$@'"
	$(VERBOSE)cd $(DOWNLOAD_DIR); wget -c $(LINUX_URL)
	$(VERBOSE)touch $@

clean:
	$(VERBOSE)rm -rf $(CONTRIB_DIR)

cleanall: clean
	$(VERBOSE)rm -rf $(DOWNLOAD_DIR)
