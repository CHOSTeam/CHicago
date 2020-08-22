#!/bin/bash

if [ "$SUBARCH" == "32" ]; then
	NOT_SUBARCH="64"
else
	NOT_SUBARCH="32"
fi

ARCH_OBJS=($(find $ROOT_DIR/kernel/arch/x86 -name *.c \
			 -o -name *.C -o -name *.cc -o -name *.cxx -o -name *.c++ \
			 -o -name *.s -o -name *.S -o -name *.asm | \
			 grep -v **${NOT_SUBARCH}.* | sed -e "s@$ROOT_DIR/kernel/@@g"))