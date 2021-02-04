# File author is Ítalo Lima Marconato Matias
#
# Created on January 28 of 2021, at 15:49 BRT
# Last edited on January 29 of 2021, at 13:15 BRT

DEBUG ?= false
VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
OUT_DIR := $(ROOT_DIR)/../../../toolchain/misc

ifneq ($(VERBOSE),true)
NOECHO := @
endif

# The .deps file will be rebuilt anyways (even if we use find instead of manually specifying the files).

OUT := build/sia-create
SOURCES := $(shell find $(ROOT_DIR) -name \*.cxx -print | sed -e "s@$(ROOT_DIR)/@@g")
OBJECTS := $(addprefix build/,$(filter %.o, $(SOURCES:%.cxx=%.o)))

CXXFLAGS := -Iinclude -flto -std=gnu++2a
LIBS := -lstdc++fs

ifeq ($(DEBUG),true)
CXXFLAGS += -g -Og
DEFS += -DDEBUG
else
CXXFLAGS += -O3
endif

build: $(OUT)

clean:
	$(NOECHO)rm -f $(OUT) $(OBJECTS) $(DEPS)

clean-all:
	$(NOECHO)rm -rf build

$(OUT): $(OBJECTS) makefile
	$(NOECHO)mkdir -p $(dir $@) $(OUT_DIR)
	$(NOECHO)echo LD: $@
	$(NOECHO)$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
	$(NOECHO)cp $(OUT) $(OUT_DIR)/

build/%.o: %.cxx makefile
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CXX: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

-include $(DEPS)
