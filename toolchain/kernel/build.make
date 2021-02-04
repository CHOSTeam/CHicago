# File author is Ítalo Lima Marconato Matias
#
# Created on January 26 of 2021, at 20:21 BRT
# Last edited on February 04 of 2021, at 17:40 BRT

# We expect all the required variables to be set by whoever included us (PATH already set, TOOLCHAIN_DIR pointing to
# where we are (and ROOT_DIR were the kernel project is).

ifeq ($(ARCH),arm64)
    FULL_ARCH := arm64

    CXX := aarch64-elf-gcc

    PRE_LIBS := $(shell $(CXX) -print-file-name=crti.o)
    LIBS := $(shell $(CXX) -print-file-name=crtn.o)

    LINK_SCRIPT := link.ld
else ifeq  ($(ARCH),x86)
    FULL_ARCH := x86
	CXX := i686-elf-gcc
	LINK_SCRIPT := link.ld
else ifeq ($(ARCH),amd64)
    VA ?= 48
    FULL_ARCH := amd64
    OUT := $(OUT)-va$(VA)

	CXX := x86_64-elf-gcc

	CXXFLAGS := -mcmodel=large -mno-red-zone
	DEFS := -DVA=$(VA)

    LINK_SCRIPT := link-va$(VA).ld
else
	$(error Invalid/unsupported architecture $(ARCH))
endif

CXXFLAGS += -Iinclude -Iarch/$(ARCH)/include -ffreestanding -fno-rtti -fno-exceptions -fno-use-cxa-atexit \
            -fno-stack-protector -std=c++2a
LDFLAGS += -nostdlib -T$(ROOT_DIR)/arch/$(ARCH)/$(LINK_SCRIPT)
PRE_LIBS := $(shell $(CXX) -print-file-name=crtbegin.o) $(PRE_LIBS)
LIBS += $(shell $(CXX) -print-file-name=crtend.o) -lgcc
DEFS += -DARCH=\"$(ARCH)\"

ifeq ($(DEBUG),true)
CXXFLAGS += -g -Og
DEFS += -DDEBUG
else
CXXFLAGS += -O3
endif

OBJECTS := $(addprefix build/$(FULL_ARCH)/arch/,$(filter %.o, $(ARCH_SOURCES:%.cxx=%.o) $(ARCH_SOURCES:%.S=%.o))) \
           $(addprefix build/$(FULL_ARCH)/,$(filter %.o, $(SOURCES:%.cxx=%.o)))
DEPS := $(OBJECTS:.o=.d)

build: $(OUT)

clean:
	$(NOECHO)rm -f $(OUT) $(OBJECTS) $(DEPS)

clean-all:
	$(NOECHO)rm -rf build

$(OUT): $(OBJECTS) makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo LD: $@
	$(NOECHO)$(CXX) $(LDFLAGS) -o $@ $(PRE_LIBS) $(OBJECTS) $(LIBS)

build/$(FULL_ARCH)/%.o: %.cxx makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CXX: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

build/$(FULL_ARCH)/arch/%.o: arch/$(ARCH)/%.cxx makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CXX: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

build/$(FULL_ARCH)/arch/%.o: arch/$(ARCH)/%.S makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo AS: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

-include $(DEPS)
