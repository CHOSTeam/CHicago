# File author is Ítalo Lima Marconato Matias
#
# Created on October 29 of 2019, at 19:46 BRT
# Last edited on November 02 of 2019, at 15:19 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

SYSROOT_DIR ?=
LIB_DIR ?= System/Libraries
INC_DIR ?= Development/Headers
PROG_DIR ?= System/Programs

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

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) LIB_DIR=$(LIB_DIR) INC_DIR=$(INC_DIR) make -C libchicago all install
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) PROG_DIR=$(PROG_DIR) make -C sesmgr all install

recompile:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) LIB_DIR=$(LIB_DIR) INC_DIR=$(INC_DIR) make -C libchicago all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) PROG_DIR=$(PROG_DIR) make -C sesmgr all

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) LIB_DIR=$(LIB_DIR) INC_DIR=$(INC_DIR) make -C libchicago clean
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) PROG_DIR=$(PROG_DIR) make -C sesmgr clean

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) LIB_DIR=$(LIB_DIR) INC_DIR=$(INC_DIR) make -C libchicago clean-all
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) PROG_DIR=$(PROG_DIR) make -C sesmgr clean-all

remake: clean all

install:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) LIB_DIR=$(LIB_DIR) INC_DIR=$(INC_DIR) make -C libchicago install
	$(NOECHO)ARCH=$(ARCH) SUBARCH=$(SUBARCH) TARGET=$(TARGET) VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) SYSROOT_DIR=$(SYSROOT_DIR) PROG_DIR=$(PROG_DIR) make -C sesmgr install
