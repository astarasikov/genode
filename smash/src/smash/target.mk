TARGET = smash
SRC_CC = main.cc
LIBS   = cxx env libssp
CC_OPT += -fPIC -fstack-protector-all