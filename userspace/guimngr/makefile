# File author is Ítalo Lima Marconato Matias
#
# Created on October 31 of 2019, at 17:51 BRT
# Last edited on December 26 of 2019, at 12:26 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

SYSROOT_DIR ?=
PROG_DIR ?= System/Programs
THEME_DIR ?= System/Themes
FONT_DIR ?= System/Fonts

ifeq ($(ARCH),x86)
	SUBARCH ?= 32
	
	ifeq ($(SUBARCH),32)
		FULL_ARCH := $(ARCH)
		TARGET ?= i686-chicago
		ARCH_CFLAGS += -msse2
	else ifeq ($(SUBARCH),64)
		FULL_ARCH := $(ARCH)-$(SUBARCH)
		TARGET ?= x86_64-chicago
		ARCH_CFLAGS += -DARCH_64
	else
		UNSUPPORTED_ARCH := true
	endif
else
	UNSUPPORTED_ARCH := true
endif

OBJECTS := display.c.o img.c.o main.c.o renderer.c.o
OBJECTS += server.c.o window.c.o

ARCH_OBJECTS := $(addprefix build/$(FULL_ARCH)/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/$(FULL_ARCH)/,$(OBJECTS))
OUTPUT := build/guimngr-$(FULL_ARCH).che

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
	$(NOECHO)rm -f $(ARCH_OBJECTS) $(OBJECTS) $(OUTPUT)

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
	$(NOECHO)echo Installing guimngr.che
	$(NOECHO)cp $(OUTPUT) $(SYSROOT_DIR)/$(PROG_DIR)/guimngr.che
	$(NOECHO)echo Installing the themes
	$(NOECHO)cp -RT Themes/ $(SYSROOT_DIR)/$(THEME_DIR)/
	$(NOECHO)echo Installing the fonts
	$(NOECHO)cp -RT Fonts/ $(SYSROOT_DIR)/$(FONT_DIR)/

$(OUTPUT): $(ARCH_OBJECTS) $(OBJECTS)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)
	$(NOECHO)$(TARGET)-gcc -nostdlib -o $@ $(ARCH_OBJECTS) $(OBJECTS) $(ARCH_LDFLAGS) -lchicago

build/$(FULL_ARCH)/%.s.o: %.s
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc $(ARCH_AFLAGS) $< -o $@

build/$(FULL_ARCH)/%.c.o: %.c
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(DEBUG),yes)
	$(NOECHO)$(TARGET)-gcc -DARCH=L\"$(ARCH)\" -DDEBUG -g -std=c11 -Iinclude -Iarch/$(ARCH)/include -fno-builtin -fshort-wchar -funroll-loops -fomit-frame-pointer -ffast-math -O0 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -DARCH=L\"$(ARCH)\" -std=c11 -Iinclude -Iarch/$(ARCH)/include -fno-builtin -fshort-wchar -funroll-loops -fomit-frame-pointer -ffast-math -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(ARCH_CFLAGS) -c $< -o $@
endif
