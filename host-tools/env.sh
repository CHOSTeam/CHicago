# File author is Ítalo Lima Marconato Matias
#
# Created on November 16 of 2020, at 10:57 BRT
# Last edited on November 28 of 2020, at 13:11 BRT

# This script is supposed to be source'd, not run, as it will setup the path and add some functions to make development easier.

if [ -z ${ARCH+z} ]; then ARCH=x86; fi
if [ -z ${TYPE+z} ]; then TYPE=Debug; fi
if [ -z ${TOOLCHAIN+z} ]; then TOOLCHAIN=/opt/chicago/toolchain; fi
if [ -z ${SYSROOT+z} ]; then SYSROOT=/opt/chicago/sysroot.$ARCH; fi

if [ ! -d ${TOOLCHAIN} ]; then
	printf "The required toolchain is still not installed, download and extract it to $TOOLCHAIN.\n"
	exit 1
fi

# Add the toolchain to the path, and check if everything we need is installed.

ROOT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")/..")
PATH=$TOOLCHAIN/bin:$PATH

if [ ! -x "$(command -v clang)" ]; then printf "The toolchain is incomplete/corrupted (clang is not avaliable).\n"; fi
if [ ! -x "$(command -v xorriso)" ]; then printf "The toolchain is incomplete/corrupted (xorriso is not avaliable).\n"; fi
if [ ! -x "$(command -v mtools)" ]; then printf "The toolchain is incomplete/corrupted (mtools is not avaliable).\n"; fi
#if [ ! -x "$(command -v sia-create)" ]; then printf "The toolchain is incomplete/corrupted (sia-create is not avaliable).\n"; fi
if [ ! -x "$(command -v cmake)" ]; then printf "The toolchain is incomplete/corrupted (cmake is not avaliable).\n"; fi
if [ ! -x "$(command -v ninja)" ]; then printf "The toolchain is incomplete/corrupted (ninja is not avaliable).\n"; fi

# Create the helper functions.

function build {
	if [ ! -d build/$ARCH ]; then
		cmake -GNinja -DCMAKE_BUILD_TYPE=$TYPE -DCMAKE_TOOLCHAIN_FILE=$ROOT_DIR/host-tools/cmake/Toolchain/$ARCH.cmake \
		      -DCHOS_ARCH=$ARCH -DCHOS_TOOLCHAIN=$TOOLCHAIN -DCHOS_SYSROOT=$SYSROOT -S . -B build/$ARCH
	fi

	cmake --build build/$ARCH
}
