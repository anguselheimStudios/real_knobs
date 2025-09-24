#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#
# Modified by Anguselheim Studios
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = real_knobs_x8

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	real_knobs_x8.cpp

# --------------------------------------------------------------
# Do some magic

include ../../DPF/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable plugin types 
# Uncomment the corresponding line to create the desired plugin type. 

#TARGETS += jack
TARGETS += lv2_dsp
#TARGETS += vst2
#TARGETS += vst3

all: $(TARGETS)

# --------------------------------------------------------------
