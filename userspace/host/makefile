# File author is Ítalo Lima Marconato Matias
#
# Created on January 28 of 2020, at 16:12 BRT
# Last edited on February 15 of 2021, at 10:15 BRT

DEBUG ?= false
VERBOSE ?= false

ifneq ($(VERBOSE),true)
NOECHO := @
endif

build:
	+$(NOECHO)make -C sia-create DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build

clean:
	+$(NOECHO)make -C sia-create VERBOSE=$(VERBOSE) clean

clean-all:
	+$(NOECHO)make -C sia-create VERBOSE=$(VERBOSE) clean-all
