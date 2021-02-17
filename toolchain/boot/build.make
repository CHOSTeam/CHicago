# File author is Ítalo Lima Marconato Matias
#
# Created on January 01 of 2021, at 15:13 BRT
# Last edited on February 17 of 2021, at 10:37 BRT

# We expect all the required variables to be set by whoever included us (PATH already set, TOOLCHAIN_DIR pointing to
# where we are, etc).

ifeq ($(ARCH),arm64)
	CC := aarch64-w64-mingw32-gcc
	LDFLAGS := -machine:arm64
	DEFS := -DELF_MACHINE=0xB7
else ifeq ($(ARCH),x86)
	CC := i686-w64-mingw32-gcc
	LDFLAGS := -machine:x86 -safeseh:no
	DEFS := -DELF_MACHINE=0x03
else ifeq ($(ARCH),amd64)
	CC := x86_64-w64-mingw32-gcc
	CFLAGS := -mno-red-zone
	LDFLAGS := -machine:x64
	DEFS := -DELF_MACHINE=0x3E
else
$(error Invalid/unsupported architecture $(ARCH))
endif

LD := lld-link
CFLAGS += -Iinclude -Iarch/$(ARCH)/include -ffreestanding -fno-stack-protector -fshort-wchar -flto -std=c2x -Wall -Wextra
LDFLAGS += -nodefaultlib -entry:EfiMain -subsystem:efi_application -dll
LIBS += $(shell $(CC) -print-libgcc-file-name)
DEFS += -DARCH=\"$(ARCH)\"

ifeq ($(DEBUG),true)
CFLAGS += -g -Og
DEFS += -DDEBUG
else
CFLAGS += -O3
endif

OBJECTS := $(addprefix build/$(ARCH)/arch/,$(addsuffix .o,$(ARCH_SOURCES))) \
           $(addprefix build/$(ARCH)/,$(addsuffix .o,$(SOURCES)))
DEPS := $(OBJECTS:.o=.d)

build: $(OUT)

clean:
	$(NOECHO)rm -f $(OUT) $(OBJECTS) $(DEPS)

clean-all:
	$(NOECHO)rm -rf build

$(OUT): $(OBJECTS) makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo LD: $@
	$(NOECHO)$(LD) $(LDFLAGS) -out:$@ $(OBJECTS) $(LIBS)

build/$(ARCH)/%.c.o: %.c makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CC: $<
	$(NOECHO)$(CC) $(CFLAGS) $(DEFS) -c -MMD -o $@ $<

build/$(ARCH)/arch/%.c.o: arch/$(ARCH)/%.c makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo CC: $<
	$(NOECHO)$(CC) $(CFLAGS) $(DEFS) -c -MMD -o $@ $<

build/$(ARCH)/arch/%.S.o: arch/$(ARCH)/%.S makefile $(TOOLCHAIN_DIR)/build.make
	$(NOECHO)mkdir -p $(dir $@)
	$(NOECHO)echo AS: $<
	$(NOECHO)$(CC) $(CFLAGS) $(DEFS) -c -MMD -o $@ $<

-include $(DEPS)
