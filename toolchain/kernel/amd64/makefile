# File author is Ítalo Lima Marconato Matias
#
# Created on December 30 of 2020, at 15:07 BRT
# Last edited on July 04 of 2020, at 15:18 BRT

VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR := $(ROOT_DIR)/out
SOURCE_DIR := $(ROOT_DIR)/../../build
PATH := $(OUT_DIR)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash
NPROC := $(shell expr $(shell nproc) \* 2 \+ 1)

# Instead of downloading/using the .tar files, we're directly cloning the source repo, and using the latest commit
# (which is probably unstable, and would probably be a better idea to use a stable branch, but whatever).

GIT_CMD := git clone --depth 1 --single-branch --recurse-submodules -j$(NPROC)
BINUTILS_REPO := git://sourceware.org/git/binutils-gdb.git
GCC_REPO := git://gcc.gnu.org/git/gcc.git

ifneq ($(VERBOSE),true)
NOECHO := @
endif

$(OUT_DIR)/bin/x86_64-elf-gcc: $(OUT_DIR)/x86_64-elf
	$(NOECHO)$(OUT_DIR)/bin/x86_64-elf-gcc -c -o \
			 $(shell dirname `$(OUT_DIR)/bin/x86_64-elf-gcc -print-libgcc-file-name`)/crti.o crti.S
	$(NOECHO)$(OUT_DIR)/bin/x86_64-elf-gcc -c -o \
			 $(shell dirname `$(OUT_DIR)/bin/x86_64-elf-gcc -print-libgcc-file-name`)/crtn.o crtn.S

$(OUT_DIR)/x86_64-elf:
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
	$(NOECHO)echo "Building Binutils"
	$(NOECHO)rm -rf $(SOURCE_DIR)/binutils/build && mkdir $(SOURCE_DIR)/binutils/build
	$(NOECHO)cd $(SOURCE_DIR)/binutils/build && \
			 ../configure --target=x86_64-elf --prefix=$(OUT_DIR) --with-sysroot --enable-gold=default --enable-lto \
			              --disable-multilib --disable-nls --disable-werror 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/binutils/build -j$(NPROC) 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/binutils/build -j$(NPROC) install 1>/dev/null 2>&1
	$(NOECHO)echo "Building GCC"
	$(NOECHO)rm -rf $(SOURCE_DIR)/gcc/build && mkdir $(SOURCE_DIR)/gcc/build
	$(NOECHO)cd $(SOURCE_DIR)/gcc/build && \
			 ../configure --target=x86_64-elf --prefix=$(OUT_DIR) --enable-languages=c,c++ --enable-lto \
			              --disable-multilib --without-headers --disable-nls --disable-werror 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/gcc/build -j$(NPROC) all-gcc all-c++tools 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/gcc/build -j$(NPROC) all-target-libgcc \
			 CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>&1
	$(NOECHO)make -C $(SOURCE_DIR)/gcc/build -j$(NPROC) install-gcc install-c++tools install-target-libgcc \
	         1>/dev/null 2>&1

clean:
	$(NOECHO)rm -rf $(OUT_DIR)
