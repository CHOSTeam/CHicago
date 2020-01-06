# File author is Ítalo Lima Marconato Matias
#
# Created on December 29 of 2019, at 19:46 BRT
# Last edited on January 05 of 2020, at 22:51 BRT

PLATFORM ?= chicago
VERBOSE ?= false
DEBUG ?= false

ifeq ($(PLATFORM),chicago)
	SYSROOT_DIR ?=
	LIB_DIR ?= System/Libraries
	INC_DIR ?= Development/Headers
	
	ARCH ?= x86
	SUBARCH ?= 32
	
	PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/$(ARCH)-$(SUBARCH)/bin:$(PATH)
	SHELL := env PATH=$(PATH) /bin/bash
	
	LIB_SUFFIX := .so
	PLATFORM_CFLAGS := -fshort-wchar
	PLATFORM_CUSTOM_ALLOC := true
	
	ifeq ($(SUBARCH),32)
		FULL_BUILD_STRING := $(PLATFORM)-$(ARCH)
		TARGET ?= i686-chicago
		PLATFORM_CFLAGS += -msse2
	else ifeq ($(SUBARCH),64)
		FULL_BUILD_STRING := $(PLATFORM)-$(ARCH)-$(SUBARCH)
		TARGET ?= x86_64-chicago
		PLATFORM_CFLAGS += -fPIC
	else
		UNSUPPORTED_PLATFORM := true
	endif
	
	PLATFORM_OBJECTS := alloc.c.o exit.c.o file.c.o lock.c.o stream.c.o virt.c.o
else
	UNSUPPORTED_PLATFORM := true
endif

ifneq ($(PLATFORM_CUSTOM_ALLOC),true)
	ALLOC_GENERIC := stdlib/alloc-generic.c.o
else
	ALLOC_GENERIC :=
endif

OBJECTS := assert/__assert.c.o

OBJECTS += stdio/__fopen_parse_mode.c.o stdio/__vprint.c.o stdio/clearerr.c.o stdio/clearerr_unlocked.c.o stdio/fclose.c.o stdio/feof.c.o stdio/feof_unlocked.c.o stdio/ferror.c.o
OBJECTS += stdio/ferror_unlocked.c.o stdio/fflush.c.o stdio/fflush_unlocked.c.o stdio/fgetc.c.o stdio/fgetc_unlocked.c.o stdio/fgetpos.c.o stdio/fgetpos_unlocked.c.o stdio/fgets.c.o
OBJECTS += stdio/fgets_unlocked.c.o stdio/flockfile.c.o stdio/fopen.c.o stdio/fprintf.c.o stdio/fprintf_unlocked.c.o stdio/fputc.c.o stdio/fputc_unlocked.c.o stdio/fputs.c.o
OBJECTS += stdio/fputs_unlocked.c.o stdio/fread.c.o stdio/fread_unlocked.c.o stdio/freopen.c.o stdio/fseek.c.o stdio/fseek_unlocked.c.o stdio/fsetpos.c.o stdio/fsetpos_unlocked.c.o
OBJECTS += stdio/ftell.c.o stdio/ftell_unlocked.c.o stdio/ftrylockfile.c.o stdio/funlockfile.c.o stdio/fwrite.c.o stdio/fwrite_unlocked.c.o stdio/getchar.c.o stdio/getchar_unlocked.c.o
OBJECTS += stdio/printf.c.o stdio/printf_unlocked.c.o stdio/putchar.c.o stdio/putchar_unlocked.c.o stdio/puts.c.o stdio/puts_unlocked.c.o stdio/rewind.c.o stdio/rewind_unlocked.c.o
OBJECTS += stdio/setbuf.c.o stdio/setbuf_unlocked.c.o stdio/setvbuf.c.o stdio/setvbuf_unlocked.c.o stdio/snprintf.c.o stdio/sprintf.c.o stdio/stdstreams.c.o stdio/ungetc.c.o
OBJECTS += stdio/ungetc_unlocked.c.o stdio/vfprintf.c.o stdio/vfprintf_unlocked.c.o stdio/vprintf.c.o stdio/vprintf_unlocked.c.o stdio/vsnprintf.c.o stdio/vsprintf.c.o

OBJECTS += stdlib/_Exit.c.o stdlib/aligned_alloc.c.o $(ALLOC_GENERIC) stdlib/abort.c.o stdlib/atexit.c.o stdlib/at_quick_exit.c.o stdlib/calloc.c.o stdlib/exit.c.o
OBJECTS += stdlib/free.c.o stdlib/malloc.c.o stdlib/quick_exit.c.o stdlib/realloc.c.o

OBJECTS += string/memcmp.c.o string/memcpy.c.o string/memmove.c.o string/memset.c.o string/strcat.c.o string/strcmp.c.o string/strcpy.c.o string/strlen.c.o
OBJECTS += string/strncat.c.o string/strncmp.c.o string/strncpy.c.o

PLATFORM_OBJECTS := $(addprefix build/$(FULL_BUILD_STRING)/platform/$(PLATFORM)/,$(PLATFORM_OBJECTS))
OBJECTS := $(addprefix build/$(FULL_BUILD_STRING)/,$(OBJECTS))
OUTPUT := build/libc-$(FULL_BUILD_STRING)$(LIB_SUFFIX)
CRT0 := build/crt0-$(FULL_BUILD_STRING).o

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(CRT0) $(OUTPUT)

clean:
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)rm -f $(PLATFORM_OBJECTS) $(OBJECTS) $(OUTPUT)

clean-all:
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)rm -rf build

remake: clean all
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif

install: all
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)echo Installing headers
	$(NOECHO)cp -RT include $(SYSROOT_DIR)/$(INC_DIR)
	$(NOECHO)cp -RT platform/$(PLATFORM)/include $(SYSROOT_DIR)/$(INC_DIR)
	$(NOECHO)echo Installing libc$(LIB_SUFFIX)
	$(NOECHO)cp $(OUTPUT) $(SYSROOT_DIR)/$(LIB_DIR)/libc$(LIB_SUFFIX)
	$(NOECHO)echo Installing crt0.o
	$(NOECHO)cp $(CRT0) $(SYSROOT_DIR)/$(DEV_LIB_DIR)/crt0.o

build/$(FULL_BUILD_STRING)/%.s.o: %.s
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -c $(PLATFORM_AFLAGS) $< -o $@

build/$(FULL_BUILD_STRING)/%.c.o: %.c
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(DEBUG),yes)
	$(NOECHO)$(TARGET)-gcc -DDEBUG -g3 -ggdb -std=c2x -Iinclude -Iplatform/$(PLATFORM)/include -fno-builtin -funroll-loops -fomit-frame-pointer -ffast-math -Og -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(PLATFORM_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -std=c2x -Iinclude -Iplatform/$(PLATFORM)/include -fno-builtin -funroll-loops -fomit-frame-pointer -ffast-math -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(PLATFORM_CFLAGS) -c $< -o $@
endif

$(CRT0): platform/$(PLATFORM)/crt0.c
ifeq ($(UNSUPPORTED_PLATFORM),true)
	$(error Unsupported platform $(PLATFORM))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(DEBUG),yes)
	$(NOECHO)$(TARGET)-gcc -DDEBUG -g3 -ggdb -std=c2x -Iinclude -Iplatform/$(PLATFORM)/include -fno-builtin -funroll-loops -fomit-frame-pointer -ffast-math -Og -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(PLATFORM_CFLAGS) -c $< -o $@
else
	$(NOECHO)$(TARGET)-gcc -std=c2x -Iinclude -Iplatform/$(PLATFORM)/include -fno-builtin -funroll-loops -fomit-frame-pointer -ffast-math -O3 -Wall -Wextra -Wno-implicit-fallthrough -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast $(PLATFORM_CFLAGS) -c $< -o $@
endif

include platform/$(PLATFORM)/platform.mk
