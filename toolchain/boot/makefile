# File author is Ítalo Lima Marconato Matias
#
# Created on December 31 of 2020, at 13:45 BRT
# Last edited on January 22 of 2021, at 15:06 BRT

VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR := $(ROOT_DIR)/out
SOURCE_DIR := $(ROOT_DIR)/../build
PATH := $(OUT_DIR)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash
NPROC := $(shell expr $(shell nproc) \* 2 \+ 1)

# Instead of downloading/using the .tar files, we're directly cloning the source repo, and using the latest commit (which
# is probably unstable, and would probably be a better idea to use a stable branch, but whatever).

GIT_CMD := git clone --depth 1 --single-branch --recurse-submodules -j$(NPROC)
LLVM_MINGW_REPO := git://github.com/mstorsjo/llvm-mingw.git

ifneq ($(VERBOSE),true)
NOECHO := @
endif

build: prepare
ifeq (,$(wildcard $(OUT_DIR)/bin))
	$(NOECHO)echo Building LLVM
	$(NOECHO)cd $(SOURCE_DIR)/llvm-mingw && ./build-llvm.sh $(OUT_DIR) 1>/dev/null 2>&1
	$(NOECHO)echo Building the wrappers
	$(NOECHO)cd $(SOURCE_DIR)/llvm-mingw &&./install-wrappers.sh $(OUT_DIR) 1>/dev/null 2>&1
	$(NOECHO)echo Building MinGW
	$(NOECHO)cd $(SOURCE_DIR)/llvm-mingw &&./build-mingw-w64.sh $(OUT_DIR) --with-default-msvcrt=msvcrt 1>/dev/null 2>&1
	$(NOECHO)echo Building Compiler-RT
	$(NOECHO)cd $(SOURCE_DIR)/llvm-mingw &&./build-compiler-rt.sh $(OUT_DIR) 1>/dev/null 2>&1
endif

prepare:
ifeq (,$(wildcard $(OUT_DIR)/bin))
ifeq (,$(wildcard $(SOURCE_DIR)/llvm-mingw))
	$(NOECHO)mkdir -p $(SOURCE_DIR)
	$(NOECHO)echo Cloning the LLVM-MinGW repo
	$(NOECHO)$(GIT_CMD) $(LLVM_MINGW_REPO) $(SOURCE_DIR)/llvm-mingw 1>/dev/null 2>&1
endif
endif

clean:
	$(NOECHO)rm -rf $(OUT_DIR)
