#
# \brief  Tests for I2C on omap4 tuna board
# \author Alexander Tarasikov
# \date   2012-09-17
#

TARGET = test-i2c-off
REQUIRES = omap4
SRC_CC += main.cc
LIBS += cxx env

vpath main.cc $(PRG_DIR)
