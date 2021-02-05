# File author is Ítalo Lima Marconato Matias
#
# Created on January 28 of 2020, at 16:05 BRT
# Last edited on February 05 of 2021, at 15:17 BRT

ARCH ?= arm64
DEBUG ?= false
VERBOSE ?= false

ifneq ($(VERBOSE),true)
NOECHO := @
endif

# And yes, we pass all special flags (like VA) for all archs, I'm not going to make arch-specific build cases here.

build:
	$(NOECHO)make -C toolchain ARCH=$(ARCH) VERBOSE=$(VERBOSE) build
	$(NOECHO)make -C boot ARCH=$(ARCH) DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build
	$(NOECHO)make -C kernel ARCH=$(ARCH) DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build
	$(NOECHO)make -C userspace ARCH=$(ARCH) DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build

clean:
	$(NOECHO)make -C boot ARCH=$(ARCH) VERBOSE=$(VERBOSE) clean
	$(NOECHO)make -C kernel ARCH=$(ARCH) VERBOSE=$(VERBOSE) clean
	$(NOECHO)make -C userspace ARCH=$(ARCH) VERBOSE=$(VERBOSE) clean

clean-all:
	$(NOECHO)make -C boot VERBOSE=$(VERBOSE) clean-all
	$(NOECHO)make -C kernel VERBOSE=$(VERBOSE) clean-all
	$(NOECHO)make -C userspace VERBOSE=$(VERBOSE) clean-all
