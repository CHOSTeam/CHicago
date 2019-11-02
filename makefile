# File author is Ítalo Lima Marconato Matias
#
# Created on September 01 of 2018, at 12:02 BRT
# Last edited on October 31 of 2019, at 18:00 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false
SYS_LANG ?= en

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

ifeq ($(ARCH),x86)
	SUBARCH ?= 32

	ifeq ($(SUBARCH),32)
		TARGET ?= i686-chicago
	else ifeq ($(SUBARCH),64)
		TARGET ?= x86_64-chicago
	else
		UNSUPPORTED_ARCH := true
	endif
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(SYS_LANG),en)
ifneq ($(SYS_LANG),br)
	UNSUPPORTED_LANG := true
endif
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: toolchain remake_sysroot
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
	$(NOECHO)BUILD_CORES=$(BUILD_CORES) ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) make -C toolchain all
ifneq ($(NOBOOT),true)
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) all
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(ROOT_DIR)/toolchain/$(ARCH)-$(SUBARCH)/sysroot make -C userspace all
ifneq ($(NOBOOT),true)
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYS_LANG=$(SYS_LANG) make finish
endif

clean: arch-clean
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
ifneq ($(NOBOOT),true)
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) clean
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C userspace clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
ifneq ($(NOBOOT),true)
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) clean-all
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel clean-all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C userspace clean-all
	$(NOECHO)rm -rf build

remake: remake_sysroot
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
ifeq ($(UNSUPPORTED_LANG),true)
	$(error Unsupported language $(SYS_LANG))
endif
ifneq ($(NOBOOT),true)
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C arch/$(ARCH) remake
endif
	$(NOECHO)SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C kernel remake
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(ROOT_DIR)/toolchain/$(ARCH)-$(SUBARCH)/sysroot make -C userspace remake
ifneq ($(NOBOOT),true)
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYS_LANG=$(SYS_LANG) make finish
endif

remake_sysroot:
	$(NOECHO)rm -rf $(ROOT_DIR)/toolchain/$(ARCH)-$(SUBARCH)/sysroot
	$(NOECHO)mkdir -p $(ROOT_DIR)/toolchain/$(ARCH)-$(SUBARCH)/sysroot/{Boot,Development,System}
	$(NOECHO)mkdir -p $(ROOT_DIR)/toolchain/$(ARCH)-$(SUBARCH)/sysroot/Development/{Headers,Libraries,Programs,Sources}
	$(NOECHO)mkdir -p $(ROOT_DIR)/toolchain/$(ARCH)-$(SUBARCH)/sysroot/System/{Configurations,Libraries,Programs}

ifneq ($(NOBOOT),true)
include arch/$(ARCH)/arch.mk
endif
