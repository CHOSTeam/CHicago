#!/bin/bash

ARCH_OBJS=($(find $ROOT_DIR/arch/x86/subarch -name *$SUBARCH.c | sed -e "s@$ROOT_DIR/arch/x86/@@g"))