# File author is Ítalo Lima Marconato Matias
#
# Created on December 29 of 2019, at 19:46 BRT
# Last edited on January 05 of 2020, at 12:55 BRT

ARCH ?= x86
VERBOSE ?= false
DEBUG ?= false

SYSROOT_DIR ?=
LIB_DIR ?= System/Libraries
INC_DIR ?= Development/Headers

ifeq ($(ARCH),x86)
	SUBARCH ?= 32
	
	ifeq ($(SUBARCH),32)
		FULL_ARCH := $(ARCH)
		TARGET ?= i686-chicago
		ARCH_CFLAGS += -msse2
	else ifeq ($(SUBARCH),64)
		FULL_ARCH := $(ARCH)-$(SUBARCH)
		TARGET ?= x86_64-chicago
		ARCH_CFLAGS += -fPIC
	else
		UNSUPPORTED_ARCH := true
	endif
else
	UNSUPPORTED_ARCH := true
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

OBJECTS += stdlib/_Exit.c.o stdlib/abort.c.o stdlib/atexit.c.o stdlib/at_quick_exit.c.o stdlib/calloc.c.o stdlib/exit.c.o stdlib/free.c.o stdlib/malloc.c.o
OBJECTS += stdlib/quick_exit.c.o stdlib/realloc.c.o

OBJECTS += string/memcmp.c.o string/memcpy.c.o string/memmove.c.o string/memset.c.o string/strcat.c.o string/strcmp.c.o string/strcpy.c.o string/strlen.c.o
OBJECTS += string/strncat.c.o string/strncmp.c.o string/strncpy.c.o

ARCH_OBJECTS := $(addprefix build/$(FULL_ARCH)/arch/$(ARCH)/,$(ARCH_OBJECTS))
OBJECTS := $(addprefix build/$(FULL_ARCH)/,$(OBJECTS))
OUTPUT := build/libc-$(FULL_ARCH).so
CRT0 := build/crt0-$(FULL_ARCH).o

ifneq ($(VERBOSE),true)
NOECHO := @
endif

PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../toolchain/$(ARCH)-$(SUBARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

all: $(CRT0) $(OUTPUT)

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
	$(NOECHO)echo Installing headers
	$(NOECHO)cp -RT include $(SYSROOT_DIR)/$(INC_DIR)
	$(NOECHO)echo Installing libc.so
	$(NOECHO)cp $(OUTPUT) $(SYSROOT_DIR)/$(LIB_DIR)/libc.so
	$(NOECHO)echo Installing crt0.o
	$(NOECHO)cp $(CRT0) $(SYSROOT_DIR)/$(DEV_LIB_DIR)/crt0.o

$(OUTPUT): $(ARCH_OBJECTS) $(OBJECTS)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)
	$(NOECHO)$(TARGET)-gcc -shared -nostdlib -o $@ $(ARCH_OBJECTS) $(OBJECTS) $(ARCH_LDFLAGS) -lchicago

build/$(FULL_ARCH)/%.s.o: %.s
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH), subarch $(SUBARCH))
endif
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)$(TARGET)-gcc -c $(ARCH_AFLAGS) $< -o $@

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

$(CRT0): crt0.c
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
