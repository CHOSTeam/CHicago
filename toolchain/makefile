# File author is Ítalo Lima Marconato Matias
#
# Created on November 11 of 2018, at 20:35 BRT
# Last edited on October 28 of 2019, at 17:16 BRT

ARCH ?= x86
VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PATH := $(ROOT_DIR)/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	SUBARCH ?= 32
	ifeq ($(SUBARCH),32)
		MAIN_TARGET := i686-chicago
		SECOND_TARGET := i686-w64-mingw32
		SECOND_TARGET_NO_LIBGCC := true
	else
		MAIN_TARGET := x86_64-chicago
		MAIN_TARGET_MCMODEL_LARGE := true
		SECOND_TARGET := x86_64-w64-mingw32
		SECOND_TARGET_NO_LIBGCC := true
	endif
else
	UNSUPPORTED_ARCH := true
endif

MAIN_TARGET_BINUTILS_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
MAIN_TARGET_GCC_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
SECOND_TARGET_BINUTILS_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
SECOND_TARGET_GCC_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
THIRD_TARGET_BINUTILS_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
THIRD_TARGET_GCC_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
FOURTH_TARGET_BINUTILS_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot
FOURTH_TARGET_GCC_SYSROOT := --with-sysroot=$(ROOT_DIR)/$(ARCH)-$(SUBARCH)/sysroot

ifeq ($(ARCH),x86)
	SECOND_TARGET_BINUTILS_SYSROOT := --with-sysroot
	SECOND_TARGET_GCC_SYSROOT :=
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

TARGETS := $(ARCH)-$(SUBARCH)/$(MAIN_TARGET)

ifneq ($(SECOND_TARGET),)
TARGETS += $(ARCH)-$(SUBARCH)/$(SECOND_TARGET)
endif

ifneq ($(THIRD_TARGET),)
TARGETS += $(ARCH)-$(SUBARCH)/$(THIRD_TARGET)
endif

ifneq ($(FOURTH_TARGET),)
TARGETS += $(ARCH)-$(SUBARCH)/$(FOURTH_TARGET)
endif

all: $(TARGETS)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf $(ARCH)-$(SUBARCH) build

remake: clean-all all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

$(ARCH)-$(SUBARCH)/$(MAIN_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)-$(SUBARCH)/$(MAIN_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ -d build ]; then rm -rf build; fi
	$(NOECHO)mkdir build
	$(NOECHO)mkdir -p $@/../sysroot/{Development,System}
	$(NOECHO)mkdir -p $@/../sysroot/Development/{Headers,Libraries,Programs,Sources}
	$(NOECHO)mkdir -p $@/../sysroot/System/{Configurations,Libraries,Programs}
	$(NOECHO)cp -RT dummy/ $@/../sysroot/Development/Headers/
	$(NOECHO)echo Downloading Binutils source code
	$(NOECHO)cd build && wget -4 -q https://ftp.gnu.org/gnu/binutils/binutils-2.33.1.tar.gz
	$(NOECHO)echo Downloading GCC source code
	$(NOECHO)cd build && wget -4 -q https://ftp.gnu.org/gnu/gcc/gcc-9.2.0/gcc-9.2.0.tar.gz
	$(NOECHO)echo Extracting Binutils source code
	$(NOECHO)cd build && tar xpf binutils-2.33.1.tar.gz
	$(NOECHO)echo Extracting GCC source code
	$(NOECHO)cd build && tar xpf gcc-9.2.0.tar.gz
	$(NOECHO)echo Patching the Binutils source code
	$(NOECHO)cd build/binutils-2.33.1 && patch -Np1 -s -i ../../binutils.patch
	$(NOECHO)echo Patching the GCC source code
	$(NOECHO)cd build/gcc-9.2.0 && patch -Np1 -s -i ../../gcc.patch
	$(NOECHO)echo Compiling Binutils \($(MAIN_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.33.1/build && cd build/binutils-2.33.1/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(MAIN_TARGET_BINUTILS_SYSROOT) --enable-shared --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install -j$(BUILD_CORES) 1>/dev/null 2>/dev/null
	$(NOECHO)echo Compiling GCC \($(MAIN_TARGET)\)
ifeq ($(MAIN_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(MAIN_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc 1>/dev/null 2>/dev/null
else ifeq ($(MAIN_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(MAIN_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
else
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(MAIN_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(MAIN_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
endif

ifneq ($(SECOND_TARGET),)
$(ARCH)-$(SUBARCH)/$(SECOND_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)-$(SUBARCH)/$(SECOND_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ ! -d build ]; then exit 1; fi
	$(NOECHO)rm -rf build/binutils-2.33.1/build build/gcc-9.2.0/build
	$(NOECHO)echo Compiling Binutils \($(SECOND_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.33.1/build && cd build/binutils-2.33.1/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(SECOND_TARGET_BINUTILS_SYSROOT) --enable-shared --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install -j$(BUILD_CORES) 1>/dev/null 2>/dev/null
	$(NOECHO)echo Compiling GCC \($(SECOND_TARGET)\)
ifeq ($(SECOND_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(SECOND_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc 1>/dev/null 2>/dev/null
else ifeq ($(SECOND_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(SECOND_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
else
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(SECOND_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(SECOND_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
endif
endif

ifneq ($(THIRD_TARGET),)
$(ARCH)-$(SUBARCH)/$(THIRD_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)-$(SUBARCH)/$(THIRD_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ ! -d build ]; then exit 1; fi
	$(NOECHO)rm -rf build/binutils-2.33.1/build build/gcc-9.2.0/build
	$(NOECHO)echo Compiling Binutils \($(THIRD_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.33.1/build && cd build/binutils-2.33.1/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(THIRD_TARGET_BINUTILS_SYSROOT) --enable-shared --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install -j$(BUILD_CORES) 1>/dev/null 2>/dev/null
	$(NOECHO)echo Compiling GCC \($(THIRD_TARGET)\)
ifeq ($(THIRD_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(THIRD_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc 1>/dev/null 2>/dev/null
else ifeq ($(THIRD_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(THIRD_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
else
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(THIRD_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(THIRD_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
endif
endif

ifneq ($(FOURTH_TARGET),)
$(ARCH)-$(SUBARCH)/$(FOURTH_TARGET):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $(ARCH)-$(SUBARCH)/$(FOURTH_TARGET) ]; then exit 1; fi
	$(NOECHO)if [ ! -d build ]; then exit 1; fi
	$(NOECHO)rm -rf build/binutils-2.33.1/build build/gcc-9.2.0/build
	$(NOECHO)echo Compiling Binutils \($(FOURTH_TARGET)\)
	$(NOECHO)mkdir build/binutils-2.33.1/build && cd build/binutils-2.33.1/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(FOURTH_TARGET_BINUTILS_SYSROOT) --enable-shared --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install -j$(BUILD_CORES) 1>/dev/null 2>/dev/null
	$(NOECHO)echo Compiling GCC \($(FOURTH_TARGET)\)
ifeq ($(FOURTH_TARGET_NO_LIBGCC),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(FOURTH_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc 1>/dev/null 2>/dev/null
else ifeq ($(FOURTH_TARGET_MCMODEL_LARGE),true)
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(FOURTH_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make all-target-libgcc -j$(BUILD_CORES) CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
else
	$(NOECHO)mkdir build/gcc-9.2.0/build && cd build/gcc-9.2.0/build && ../configure --target=$(FOURTH_TARGET) --prefix=$(ROOT_DIR)/$(ARCH)-$(SUBARCH) $(FOURTH_TARGET_GCC_SYSROOT) --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-nls --disable-werror 1>/dev/null 2>/dev/null && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>/dev/null && make install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
endif
endif
