# File author is Ítalo Lima Marconato Matias
#
# Created on January 28 of 2020, at 16:05 BRT
# Last edited on February 04 of 2021, at 15:55 BRT

ARCH ?= arm64
DEBUG ?= false
VERBOSE ?= false

# Only handle the ARCHs with special flags here (like arm64 and amd64, as they have the VA flag). Proper supported arch
# support should be handled inside each module.

ifeq ($(ARCH),arm64)
	VA ?= 39
else ifeq ($(ARCH),amd64)
	VA ?= 48
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

# And yes, we pass all special flags (like VA) for all archs, I'm not going to make arch-specific build cases here.

build:
	$(NOECHO)make -C toolchain ARCH=$(ARCH) VERBOSE=$(VERBOSE) build
	$(NOECHO)make -C boot ARCH=$(ARCH) DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build
	$(NOECHO)make -C kernel VA=$(VA) ARCH=$(ARCH) DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build
	$(NOECHO)make -C userspace ARCH=$(ARCH) DEBUG=$(DEBUG) VERBOSE=$(VERBOSE) build

clean:
	$(NOECHO)make -C boot ARCH=$(ARCH) VERBOSE=$(VERBOSE) clean
	$(NOECHO)make -C kernel VA=$(VA) ARCH=$(ARCH) VERBOSE=$(VERBOSE) clean
	$(NOECHO)make -C userspace ARCH=$(ARCH) VERBOSE=$(VERBOSE) clean

clean-all:
	$(NOECHO)make -C boot VERBOSE=$(VERBOSE) clean-all
	$(NOECHO)make -C kernel VERBOSE=$(VERBOSE) clean-all
	$(NOECHO)make -C userspace VERBOSE=$(VERBOSE) clean-all
