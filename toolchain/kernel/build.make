# File author is Ítalo Lima Marconato Matias
#
# Created on January 26 of 2021, at 20:21 BRT
# Last edited on March 06 of 2021, at 21:46 BRT

# We expect all the required variables to be set by whoever included us (PATH already set, TOOLCHAIN_DIR pointing to
# where we are, etc).

NM ?= nm

ifeq  ($(ARCH),x86)
    # We use -march=haswell on both x86 and amd64 because this is the minimum processor line with all the features that
    # we need.

    FULL_ARCH := x86
    
    CXX := i686-elf-gcc
    AR := i686-elf-gcc-ar
    
    CXXFLAGS := -march=haswell
    LINK_SCRIPT := link.ld
else ifeq ($(ARCH),amd64)
    FULL_ARCH := amd64
    
    CXX := x86_64-elf-gcc
    AR := x86_64-elf-gcc-ar
    
    CXXFLAGS := -mcmodel=large -mno-red-zone -march=haswell
    LINK_SCRIPT := link.ld
else
    $(error Invalid/unsupported architecture $(ARCH))
endif

CXXFLAGS += -Iinclude -Iarch/$(ARCH)/include -ffreestanding -fno-rtti -fno-exceptions -fno-use-cxa-atexit \
            -fstack-protector-all -fno-omit-frame-pointer -funroll-loops -ftree-vectorize -flax-vector-conversions \
            -std=c++20 -Wall -Wextra -Wno-implicit-fallthrough -O3
LDFLAGS += -nostdlib -Tarch/$(ARCH)/$(LINK_SCRIPT) -L. -zmax-page-size=4096 -n
DEFS += -DARCH=\"$(ARCH)\"

ifneq ($(TYPE),lib)
    LIBS += $(shell $(CXX) -print-file-name=crtend.o) ../lib/build/$(FULL_ARCH)/libkernel.a
    CXXFLAGS := -I../lib/include -I../lib/arch/$(ARCH)/include $(CXXFLAGS)
    PRE_LIBS := $(shell $(CXX) -print-file-name=crtbegin.o) $(PRE_LIBS)
    DEFS += -DKERNEL

    ifneq (true,false)
        PRE_LIBS := $(shell $(CXX) -print-file-name=crti.o) $(PRE_LIBS)
        LIBS += $(shell $(CXX) -print-file-name=crtn.o)
    else
        DEFS += -DUSE_INIT_ARRAY
    endif
endif

ifeq ($(DEBUG),true)
CXXFLAGS += -fsanitize=undefined
DEFS += -DDEBUG
endif

OBJECTS := $(addprefix build/$(FULL_ARCH)/arch/,$(addsuffix .o,$(ARCH_SOURCES))) \
           $(addprefix build/$(FULL_ARCH)/,$(addsuffix .o,$(SOURCES)))
DEPS := $(OBJECTS:.o=.d)

build: $(OUT)

clean:
	$(NOECHO)rm -f $(OUT) $(OBJECTS) $(DEPS)

clean-all:
	$(NOECHO)rm -rf build

ifeq ($(TYPE),lib)
$(OUT): $(OBJECTS) makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo AR: $@
	$(NOECHO)$(AR) -crs $(OUT) $(OBJECTS)
else
$(OUT): $(OBJECTS) ../lib/build/$(ARCH)/libkernel.a arch/$(ARCH)/$(LINK_SCRIPT) common.ld makefile \
        $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo LD: $@
	$(NOECHO)$(CXX) $(LDFLAGS) -o $@ $(PRE_LIBS) $(OBJECTS) $(LIBS) -lgcc
	$(NOECHO)$(NM) -nCS --defined $(OUT) | awk '{ if (length($$2) > 1 && $$3 != "b" && $$3 != "B" && \
	                                                  $$3 != "d" && $$3 != "D" && $$3 != "g" && $$3 != "G" && \
	                                                  $$3 != "r" && $$3 != "R" && $$3 != "s" && $$3 != "S") print; }' \
	                                     > $(OUT).syms.pre
	$(NOECHO)wc -l < $(OUT).syms.pre > $(OUT).syms
	$(NOECHO)cat $(OUT).syms.pre >> $(OUT).syms
	$(NOECHO)rm $(OUT).syms.pre
endif

build/$(FULL_ARCH)/%.cxx.o: %.cxx makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CXX: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

build/$(FULL_ARCH)/arch/%.cxx.o: arch/$(ARCH)/%.cxx makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CXX: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

build/$(FULL_ARCH)/arch/%.S.o: arch/$(ARCH)/%.S makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo AS: $<
	$(NOECHO)$(CXX) $(CXXFLAGS) $(DEFS) -c -MMD -o $@ $<

-include $(DEPS)
