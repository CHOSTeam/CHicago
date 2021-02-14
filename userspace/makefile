# File author is Ítalo Lima Marconato Matias
#
# Created on January 28 of 2020, at 16:10 BRT
# Last edited on February 14 of 2021, at 18:38 BRT

ARCH ?= amd64
DEBUG ?= false
VERBOSE ?= false

ifneq ($(VERBOSE),true)
NOECHO := @
endif

build:
	$(NOECHO)make -C host DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build

clean:
	$(NOECHO)make -C host VERBOSE=$(VERBOSE) clean

clean-all:
	$(NOECHO)make -C host VERBOSE=$(VERBOSE) clean-all
