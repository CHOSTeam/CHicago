# File author is Ítalo Lima Marconato Matias
#
# Created on October 31 of 2019, at 17:51 BRT
# Last edited on January 05 of 2020, at 18:52 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

SYSROOT_DIR ?=
PROG_DIR ?= System/Programs

ifeq ($(ARCH),x86)
	SUBARCH ?= 32
	
	ifeq ($(SUBARCH),32)
		FULL_ARCH := $(ARCH)
		TARGET ?= i686-chicago
		ARCH_CFLAGS += -msse2
	else ifeq ($(SUBARCH),64)
		FULL_ARCH := $(ARCH)-$(SUBARCH)
		TARGET ?= x86_64-chicago
	else
		UNSUPPORTED_ARCH := true
	endif
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := main.c.o

OBJECTS := $(addprefix build/$(FULL_ARCH)/,$(OBJECTS))
OUTPUT := build/osmngr-$(FULL_ARCH).che

ifneq ($(VERBOSE),true)
NOECHO := @
endif

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/$(ARCH)-$(SUBARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

all: $(OUTPUT)

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)rm -f $(OBJECTS) $(OUTPUT)

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif

install: all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Installing osmngr.che
	$(NOECHO)cp $(OUTPUT) $(SYSROOT_DIR)/$(PROG_DIR)/osmngr.che

$(OUTPUT): $(OBJECTS)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)
	$(NOECHO)$(TARGET)-gcc -o $@ $(OBJECTS) $(ARCH_LDFLAGS)

build/$(FULL_ARCH)/%.c.o: %.c
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(DEBUG),yes)
	$(NOECHO)$(TARGET)-gcc -DARCH=L\"$(ARCH)\" -DDEBUG -g3 -ggdb -std=c2x -Iinclude -Iarch/$(ARCH)/include -fno-builtin -fshort-wchar -funroll-loops -fomit-frame-pointer -ffast-math -Og -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -DARCH=L\"$(ARCH)\" -std=c2x -Iinclude -Iarch/$(ARCH)/include -fno-builtin -fshort-wchar -funroll-loops -fomit-frame-pointer -ffast-math -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
endif
