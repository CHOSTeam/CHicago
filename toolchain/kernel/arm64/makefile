# File author is Ítalo Lima Marconato Matias
#
# Created on December 30 of 2020, at 15:07 BRT
# Last edited on January 22 of 2021, at 15:08 BRT

VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR := $(ROOT_DIR)/out
SOURCE_DIR := $(ROOT_DIR)/../../build
PATH := $(OUT_DIR)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash
NPROC := $(shell expr $(shell nproc) \+ 2)

# Instead of downloading/using the .tar files, we're directly cloning the source repo, and using the latest commit
# (which is probably unstable, and would probably be a better idea to use a stable branch, but whatever).

GIT_CMD := git clone --depth 1 --single-branch --recurse-submodules -j$(NPROC)
BINUTILS_REPO := git://sourceware.org/git/binutils-gdb.git
GCC_REPO := git://gcc.gnu.org/git/gcc.git

ifneq ($(VERBOSE),true)
NOECHO := @
endif

build: prepare
ifeq (,$(wildcard $(OUT_DIR)/aarch64-elf))
	$(NOECHO)echo "Building Binutils"
	$(NOECHO)rm -rf $(SOURCE_DIR)/binutils/build && mkdir $(SOURCE_DIR)/binutils/build
	$(NOECHO)cd $(SOURCE_DIR)/binutils/build && \
			 ../configure --target=aarch64-elf --prefix=$(OUT_DIR) --with-sysroot --disable-multilib --disable-nls \
			 			  --disable-werror 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/binutils/build -j$(NPROC) 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/binutils/build -j$(NPROC) install 1>/dev/null 2>&1
	$(NOECHO)echo "Building GCC"
	$(NOECHO)rm -rf $(SOURCE_DIR)/gcc/build && mkdir $(SOURCE_DIR)/gcc/build
	$(NOECHO)cd $(SOURCE_DIR)/gcc/build && \
			 ../configure --target=aarch64-elf --prefix=$(OUT_DIR) --enable-languages=c,c++ --disable-multilib \
			 			  --without-headers --disable-nls --disable-werror 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/gcc/build -j$(NPROC) all-gcc all-target-libgcc 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/gcc/build -j$(NPROC) install-gcc install-target-libgcc 1>/dev/null 2>&1
endif

prepare:
ifeq (,$(wildcard $(OUT_DIR)/aarch64-elf))
ifeq (,$(wildcard $(SOURCE_DIR)/binutils))
	$(NOECHO)mkdir -p $(SOURCE_DIR)
	$(NOECHO)echo Cloning the Binutils repo
	$(NOECHO)$(GIT_CMD) $(BINUTILS_REPO) $(SOURCE_DIR)/binutils 1>/dev/null 2>&1
endif
ifeq (,$(wildcard $(SOURCE_DIR)/gcc))
	$(NOECHO)mkdir -p $(SOURCE_DIR)
	$(NOECHO)echo Cloning the GCC repo
	$(NOECHO)$(GIT_CMD) $(GCC_REPO) $(SOURCE_DIR)/gcc 1>/dev/null 2>&1
endif
endif

clean:
	$(NOECHO)ls -Q | grep -v makefile | xargs rm -rf
