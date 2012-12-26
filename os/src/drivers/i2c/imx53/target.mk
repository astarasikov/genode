#
# \brief  I2C specific for i.MX53
# \author Nikolay Golikov
# \date   2012-11-26
#

TARGET   = imx53_i2c_drv

REQUIRES = imx53
SRC_CC   = main.cc
LIBS     = cxx env server
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)

