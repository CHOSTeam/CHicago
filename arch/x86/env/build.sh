#!/bin/bash

force_rebuild() {
	printf "0"
}

get_ext() {
	local type=$1
	
	if [ "$type" == "app" ]; then
		printf ".che"
	elif [ "$type" == "boot" ]; then
		printf ".efi"
	elif [ "$type" == "drv" ]; then
		printf ".chd"
	elif [ "$type" == "lib" ]; then
		printf ".so"
	fi
}

get_incs() {
	declare spath=$1
	declare name=$2
	declare type=$3
	
	# If this isn't a .c file, we're not going to get the deps from it
	
	local ext=${name##*.}
	
	if [ "$ext" == "asm" ] || [ "$ext" == "s" ] || [ "$ext" == "S" ]; then
		return
	fi
	
	if [ "$type" == "kernel" ] || [ "$type" == "drv" ]; then
		printf -- "-ffreestanding -I$spath/include -I$spath/arch/x86/include -I$spath/arch/x86/include$SUBARCH"
	elif [ "$type" == "boot" ]; then
		printf -- "-ffreestanding -I$spath/include -I$spath/subarch/$SUBARCH/include -I$TOOLCHAIN/include/efi -I$TOOLCHAIN/include/efi/protocol"
		
		if [ "$SUBARCH" == "64" ]; then
			printf -- " -I$TOOLCHAIN/include/efi/x86_64"
		else
			printf -- " -I$TOOLCHAIN/include/efi/ia32"
		fi
	fi
}

get_deps() {
	declare spath=$1
	declare name=$2
	declare type=$3
	
	# If this isn't a .c file, we're not going to check it
	
	local ext=${name##*.}
	
	if [ "$ext" == "asm" ] || [ "$ext" == "s" ] || [ "$ext" == "S" ]; then
		printf "$spath/$name"
		return
	fi
	
	# The compiler for the EFI bootloader isn't the default -chicago one
	
	if [ "$type" == "boot" ]; then
		local cc="${TARGET_EFI}gcc"
	else
		local cc="${TARGET}gcc"
	fi
	
	$cc -M $(get_incs $spath $name $type) $spath/$name
}

compile() {
	declare spath=$1
	declare opath=$2
	declare name=$3
	declare type=$4
	
	# Make sure the destination path exists
	
	mkdir -p $(dirname $opath/$name.o)
	
	# The compiler for the EFI bootloader isn't the default -chicago one
	
	if [ "$type" == "boot" ]; then
		local as="${TARGET_EFI}as"
		local cc="${TARGET_EFI}gcc"
	else
		local as="${TARGET}as"
		local cc="${TARGET}gcc"
	fi
	
	# Get all the compilation flags
	
	local ext=${name##*.}
	local incs=$(get_incs $spath $name $type)
	local defs="-DARCH=\"$FULL_ARCH\" -DVERSION=\"$VERSION\""
	local opts="-funroll-loops -ffast-math"
	local warns="-Wall -Wextra -Wno-implicit-fallthrough"
	
	if [ "$ext" == "c" ]; then
		opts+=" -std=c2x"
		warns+=" -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-int-conversion"
	elif [ "$ext" != "asm" ] && [ "$ext" != "s" ] && [ "$ext" != "S" ]; then
		opts+=" -std=c++2a"
		
		if [ "$type" == "boot" ] || [ "$type" == "drv" ] || [ "$type" == "kernel" ]; then
			opts+=" -fno-rtti -fno-exceptions -fno-use-cxa-atexit"
		fi
	fi
	
	if [ "$SUBARCH" == "64" ]; then
		defs+=" -DARCH_64"
	fi
	
	if [ "$DEBUG" == "yes" ]; then
		defs+=" -DDBG"
		opts+=" -g -Og"
	else
		opts+=" -O3"
	fi
	
	if [ "$type" == "kernel" ]; then
		defs+=" -DKERNEL"
	fi
	
	if [ "$type" == "boot" ] || [ "$type" == "drv" ] || [ "$type" == "kernel" ]; then
		opts+=" -fno-plt -no-pie -fno-pic -ffreestanding -fno-stack-protector"
	fi
	
	if [ "$type" == "drv" ] || [ "$type" == "kernel" ]; then
		if [ "$type" == "kernel" ]; then
			if [ "$SUBARCH" == "64" ]; then
				defs+=" -DELF_MACHINE=62"
			else
				defs+=" -DELF_MACHINE=3"
			fi
		fi
		
		if [ "$SUBARCH" == "64" ]; then
			opts+=" -mcmodel=large -mno-red-zone"
		fi
		
		opts+=" -msse -msse2 -msse3 -mssse3 -mfpmath=sse"
	fi
	
	# And now let's compile!
	
	if [ "$ext" == "asm" ] || [ "$ext" == "s" ] || [ "$ext" == "S" ]; then
		printf "AS: $name\n"
		
		if [ "$SUBARCH" == "64" ]; then
			$as --defsym ARCH_64=1 -o $opath/$name.o $spath/$name
		else
			$as -o $opath/$name.o $spath/$name
		fi
	else
		printf "CC: $name\n"
		$cc $extra $defs $incs $opts $warns -c -o $opath/$name.o $spath/$name
	fi
	
	return $?
}

link() {
	declare type=$1
	declare name=$2
	declare objs="${@:3}"
	
	# Make sure the destination path exists
	
	mkdir -p $(dirname $name)
	
	# The linker for the EFI bootloader isn't the default -chicago one
	
	if [ "$type" == "boot" ]; then
		local ld="${TARGET_EFI}gcc"
	else
		local ld="${TARGET}gcc"
		local cc="${TARGET}gcc"
	fi
	
	# We (may) need to get the crtbegin and the crti, else, we gonna have some problems...
	
	if [ "$type" == "drv" ] || [ "$type" == "kernel" ]; then
		local crtbegin="$($cc -print-file-name=crti.o) $($cc -print-file-name=crtbegin.o)"
		local crtend="$($cc -print-file-name=crtend.o) $($cc -print-file-name=crtn.o)"
	fi
	
	# Get all the linker flags
	
	if [ "$type" == "boot" ] || [ "$type" == "drv" ] || [ "$type" == "kernel" ]; then
		local flags="-ffreestanding"
		local after="-lgcc"
		
		if [ "$type" == "boot" ]; then
			flags+=" -shared -e $([ \"$SUBARCH\" == \"32\" ] && printf _)efi_main -Wl,-dll -Wl,--subsystem,10 -L$TOOLCHAIN/lib"
			after+=" -lefi"
		else
			flags+=" -fno-rtti -fno-exceptions"
		fi
		
		if [ "$type" == "drv" ]; then
			flags+=" -fno-plt -r"
		else
			flags+=" -fno-plt -no-pie -fno-pic -nostdlib"
		fi
		
		if [ "$type" == "kernel" ]; then
			flags+=" -T$ROOT_DIR/kernel/arch/x86/link$SUBARCH.ld"
		fi
	fi
	
	# And now let's link it!
	
	printf "LD: $(basename $name)\n"
	
	$ld $flags -o $name $crtbegin $objs $extra $crtend $after
	return $?
}
