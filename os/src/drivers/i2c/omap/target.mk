#
# \brief  I2C specific for (currently only) OMAP4 systems
# \author Alexander Tarasikov
# \date   2012-09-18
#

TARGET   = omap_i2c_drv

#FIXME: make this depend on generic 'omap' platform when we implement omap3/5
REQUIRES = omap4
SRC_CC   = main.cc
LIBS     = cxx env server
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)

