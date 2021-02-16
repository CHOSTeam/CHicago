# File author is Ítalo Lima Marconato Matias
#
# Created on January 01 of 2020, at 13:25 BRT
# Last edited on February 15 of 2021, at 10:16 BRT

ARCH ?= amd64
VERBOSE ?= false

ifneq ($(VERBOSE),true)
NOECHO := @
endif

build:
	+$(NOECHO)make -C kernel/$(ARCH) VERBOSE=$(VERBOSE) build
	+$(NOECHO)make -C boot VERBOSE=$(VERBOSE) build

build-all:
	+$(NOECHO)for i in kernel/*/; do make -C $$i VERBOSE=$(VERBOSE) build; done
	+$(NOECHO)make -C boot VERBOSE=$(VERBOSE) build

clean:
	+$(NOECHO)make -C kernel/$(ARCH) VERBOSE=$(VERBOSE) clean
	+$(NOECHO)make -C boot VERBOSE=$(VERBOSE) clean

clean-all:
	+$(NOECHO)for i in kernel/*/; do make -C $$i VERBOSE=$(VERBOSE) clean; done
	+$(NOECHO)make -C boot VERBOSE=$(VERBOSE) clean
