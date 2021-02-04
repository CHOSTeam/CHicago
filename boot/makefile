# File author is Ítalo Lima Marconato Matias
#
# Created on January 01 of 2021, at 13:48 BRT
# Last edited on February 04 of 2021, at 15:56 BRT

ARCH ?= arm64
DEBUG ?= false
VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
TOOLCHAIN_DIR := $(ROOT_DIR)/../toolchain/boot
PATH := $(TOOLCHAIN_DIR)/out/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifneq ($(VERBOSE),true)
NOECHO := @
endif

# The .deps file will be rebuilt anyways (even if we use find instead of manually specifying the files).

OUT := build/$(ARCH)/osldr.efi
ARCH_SOURCES := $(shell find $(ROOT_DIR)/arch/$(ARCH) -name \*.c -print -o -name \*.S -print | \
													  sed -e "s@$(ROOT_DIR)/arch/$(ARCH)/@@g")
SOURCES := $(shell find $(ROOT_DIR) -path $(ROOT_DIR)/arch -prune -o -name \*.c -print -o -name \*.S -print | \
									sed -e "s@$(ROOT_DIR)/@@g")

# Include the arch-specific toolchain file BEFORE anything else (but after defining the SOURCES variable, as it will gen
# the object list based on it, and the makefile.deps file as well).

include $(TOOLCHAIN_DIR)/build.make
