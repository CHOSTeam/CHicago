#!/bin/bash

# Version of the arch-specific tools

GNUEFI_VERSION="3.0.12"
MINGW_VERSION="7.0.0"

# Setup the subarch, the toolchain path, and the target

if [ -z ${SUBARCH+x} ]; then SUBARCH="32"; fi

if [ "$SUBARCH" == "64" ]; then
	FULL_ARCH="x86-64"
	TARGET="x86_64-chicago-"
	TARGET_2="x86_64-chicago"
	TARGET_EFI="x86_64-w64-mingw32-"
	TARGET_EFI_2="x86_64-w64-mingw32"
else
	FULL_ARCH="x86"
	TARGET="i686-chicago-"
	TARGET_2="i686-chicago"
	TARGET_EFI="i686-w64-mingw32-"
	TARGET_EFI_2="i686-w64-mingw32"
fi

if [ -z ${TOOLCHAIN+x} ]; then TOOLCHAIN="$HOME/toolchain/$FULL_ARCH"; fi

# Export the toolchain into the PATH

export PATH=$TOOLCHAIN/bin:${PATH}

_build_binutils() {
	# Building binutils: We are already inside of the binutils source folder, the destination folder is also
	# already clean, so, let's create the build folder, run the configure script, and make/make install!
	# The only thing that changes between x86-32 and x86-64 is the target.
	# We need to build two different binutils (and so two different GCCs): The EFI version, and the normal ELF version.
	# The EFI version is used for building the EFI boot loader, and the normal version, for the kernel and the userspace.
	
	mkdir build && cd build
	
	printf "Building Binutils ($TARGET_2)\n"
	../configure --target=$TARGET_2 --prefix=$TOOLCHAIN --with-sysroot=$TOOLCHAIN/sysroot --enable-shared --disable-multilib --disable-nls --disable-werror 1>/dev/null 2>/dev/null
	make -j$(nproc) 1>/dev/null 2>/dev/null
	make -j$(nproc) install 1>/dev/null 2>/dev/null
	rm -rf *
	
	printf "Building Binutils ($TARGET_EFI_2)\n"
	../configure --target=$TARGET_EFI_2 --prefix=$TOOLCHAIN --with-sysroot=$TOOLCHAIN --enable-shared --disable-multilib --disable-nls --disable-werror 1>/dev/null 2>/dev/null
	make -j$(nproc) 1>/dev/null 2>/dev/null
	make -j$(nproc) install 1>/dev/null 2>/dev/null
	cd ../ && rm -rf build
}

_build_gcc() {
	# Before building GCC, we need to build the mingw-w64 headers, else, GCC compilation is going to fail
	
	printf "Downloading mingw-w64 source code\n"
	wget -4 -q https://downloads.sourceforge.net/project/mingw-w64/mingw-w64/mingw-w64-release/mingw-w64-v$MINGW_VERSION.tar.bz2
	
	printf "Extracting mingw-w64 source code\n"
	tar xpf mingw-w64-v$MINGW_VERSION.tar.bz2
	
	mkdir mingw-w64-v$MINGW_VERSION/build && cd mingw-w64-v$MINGW_VERSION/build
	
	printf "Building mingw-w64 ($TARGET_EFI_2)\n"
	../mingw-w64-headers/configure --host=$TARGET_EFI_2 --prefix=$TOOLCHAIN/$TARGET_EFI_2 1>/dev/null 2>/dev/null
	make -j5 install 1>/dev/null 2>/dev/null
	ln -s $TOOLCHAIN/$TARGET_EFI_2 $TOOLCHAIN/mingw
	rm -rf *
	cd ../..
	
	_build_gcc_int
}

# The compilation flags for x86-32 and x86-64 differs a bit when compiling GCC, so let's create separate build functions for each

if [ "${SUBARCH}" == "32" ]; then
	_build_gcc_int() {
		# Building gcc: Here on x86(-32) it's pretty similar to building binutils, but, of course, the configure script flags
		# are different, and, instead of just building everything, we need to build just the 'all-gcc' target, and, under the
		# ELF -chicago target, the 'all-target-libgcc' target.
		
		mkdir build && cd build
		
		printf "Building GCC (i686-chicago)\n"
		../configure --target=i686-chicago --prefix=$TOOLCHAIN --with-sysroot=$TOOLCHAIN/sysroot --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-multilib --disable-nls --disable-werror CFLAGS_FOR_TARGET='-fno-plt' 1>/dev/null 2>/dev/null
		make -j$(nproc) all-gcc all-target-libgcc 1>/dev/null 2>/dev/null
		make -j$(nproc) install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
		rm -rf *
		
		printf "Building GCC (i686-w64-mingw32)\n"
		../configure --target=i686-w64-mingw32 --prefix=$TOOLCHAIN --with-sysroot=$TOOLCHAIN --enable-languages=c,c++ --enable-fully-dynamic-strings --enable-shared --enable-default-pie --disable-win32-registry --disable-multilib --disable-sjlj-exceptions --disable-nls --disable-werror 1>/dev/null 2>/dev/null
		make -j$(nproc) all-gcc all-target-libgcc 1>/dev/null 2>/dev/null
		make -j$(nproc) install-gcc 1>/dev/null 2>/dev/null
		
		# Before building the libgcc on the mingw32 GCC, we need to build the mingw crt
		
		cd ../mingw-w64-v$MINGW_VERSION/build
		../mingw-w64-crt/configure --prefix=$TOOLCHAIN/$TARGET_EFI_2 --with-sysroot=$TOOLCHAIN/$TARGET_EFI_2 --host=$TARGET_EFI_2 1>/dev/null 2>/dev/null
		make -j$(nproc) 1>/dev/null 2>/dev/null
		make -j$(nproc) install 1>/dev/null 2>/dev/null
		
		# Finally, finish building the libgcc
		
		cd ../../build
		make -j$(nproc) all-target-libgcc 1>/dev/null 2>/dev/null
		make -j$(nproc) install-target-libgcc 1>/dev/null 2>/dev/null
		
		cd ../ && rm -rf build
		
		# And now, build the crti/crtn files (because the default ones are, like, empty?)
		
		${TARGET}as -o "$(${TARGET}gcc --print-file-name=crti.o)" $ROOT_DIR/arch/x86/env/crt/crti.S
		${TARGET}as -o "$(${TARGET}gcc --print-file-name=crtn.o)" $ROOT_DIR/arch/x86/env/crt/crtn.S
	}
elif [ "${SUBARCH}" == "64" ]; then
	_build_gcc_int() {
		# Building gcc: x86-64 is pretty similar to x86-32, BUT, the -chicago target NEEDS to be built without the red-zone, and in
		# mcmodel=large.
		
		mkdir build && cd build
		
		printf "Building GCC (x86_64-chicago)\n"
		../configure --target=x86_64-chicago --prefix=$TOOLCHAIN --with-sysroot=$TOOLCHAIN/sysroot --enable-languages=c,c++ --enable-shared --enable-default-pie --disable-multilib --disable-nls --disable-werror CFLAGS_FOR_TARGET='-fno-plt' 1>/dev/null 2>/dev/null
		make -j$(nproc) all-gcc 1>/dev/null 2>/dev/null
		make -j$(nproc) all-target-libgcc CFLAGS_FOR_TARGET='-fno-plt -g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>/dev/null
		make -j$(nproc) install-gcc install-target-libgcc 1>/dev/null 2>/dev/null
		rm -rf *
		
		printf "Building GCC (x86_64-w64-mingw32)\n"
		../configure --target=x86_64-w64-mingw32 --prefix=$TOOLCHAIN --with-sysroot=$TOOLCHAIN --enable-languages=c,c++ --enable-fully-dynamic-strings --enable-shared --enable-default-pie --disable-win32-registry --disable-multilib --disable-sjlj-exceptions --disable-nls --disable-werror 1>/dev/null 2>/dev/null
		make -j$(nproc) all-gcc 1>/dev/null 2>/dev/null
		make -j$(nproc) all-target-libgcc CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' 1>/dev/null 2>/dev/null
		make -j$(nproc) install-gcc 1>/dev/null 2>/dev/null
		
		# Before building the libgcc on the mingw32 GCC, we need to build the mingw crt
		
		cd ../mingw-w64-v$MINGW_VERSION/build
		../mingw-w64-crt/configure --prefix=$TOOLCHAIN/$TARGET_EFI_2 --with-sysroot=$TOOLCHAIN/$TARGET_EFI_2 --host=$TARGET_EFI_2 1>/dev/null 2>/dev/null
		make -j$(nproc) 1>/dev/null 2>/dev/null
		make -j$(nproc) install 1>/dev/null 2>/dev/null
		
		# Finally, finish building the libgcc
		
		cd ../../build
		make -j$(nproc) all-target-libgcc 1>/dev/null 2>/dev/null
		make -j$(nproc) install-target-libgcc 1>/dev/null 2>/dev/null
		
		cd ../ && rm -rf build
		
		# And now, build the crti/crtn files (because the default ones are, like, empty?
		
		${TARGET}as --defsym ARCH_64=1 -o "$(${TARGET}gcc --print-file-name=crti.o)" $ROOT_DIR/arch/x86/env/crt/crti.S
		${TARGET}as --defsym ARCH_64=1 -o "$(${TARGET}gcc --print-file-name=crtn.o)" $ROOT_DIR/arch/x86/env/crt/crtn.S
	}
else
	# And, of course, make sure that the user will not pass an invalid subarch
	printf "Invalid sub-architecture (${SUBARCH}).\n"
	exit 1
fi

build_gnuefi() {
	# Building gnu-efi: Determine the arch, create the destination folder, download the source code and extract it (no need to patch it)
	
	if [ "$SUBARCH" == "32" ]; then
		local arch="ia32"
	else
		local arch="x86_64"
	fi
	
	mkdir -p $TOOLCHAIN/build
	cd $TOOLCHAIN/build
	
	printf "Downloading GNU-EFI source code\n"
	wget -4 -q http://downloads.sourceforge.net/project/gnu-efi/gnu-efi-$GNUEFI_VERSION.tar.bz2
	
	printf "Extracting GNU-EFI source code\n"
	tar xpf gnu-efi-$GNUEFI_VERSION.tar.bz2
	
	# Let's go into the source folder
	
	cd gnu-efi-$GNUEFI_VERSION
	
	# And build it
	
	printf "Building GNU-EFI ($TARGET_EFI_2)\n"
	make CROSS_COMPILE=$TARGET_EFI ARCH=$arch PREFIX=$TOOLCHAIN lib inc 1>/dev/null 2>/dev/null
	make -Clib CROSS_COMPILE=$TARGET_EFI ARCH=$arch PREFIX=$TOOLCHAIN install 1>/dev/null 2>/dev/null
	make -Cinc CROSS_COMPILE=$TARGET_EFI ARCH=$arch PREFIX=$TOOLCHAIN install 1>/dev/null 2>/dev/null
	
	cd $ROOT_DIR
	rm -rf $TOOLCHAIN/build
	
	# Finally, write the installed version into the 'arch_built' file
	
	printf "$GNUEFI_VERSION\n" > $TOOLCHAIN/arch_built
}

build_arch_toolchain() {
	build_gnuefi
}

check_arch_toolchain() {
	ret=0
	
	# Check if the installed version of GNU-EFI is the recommended one
	
	if [ ! -f $TOOLCHAIN/arch_built ] || [ "${GNUEFI_VERSION}" != "$(head -n 1 $TOOLCHAIN/arch_built)" ]; then
		printf "The build environment detected that the installed version of GNU-EFI isn't the recommended one.\n"
		printf "Do you want to install the recommended version?\n"
		printf "[Y/N] "
		read ans
		printf "\n"
		
		if [ "$ans" != "${ans#[Yy]}" ]; then
			build_gnuefi
			ret=1
		fi
	fi
	
	return $ret
}
