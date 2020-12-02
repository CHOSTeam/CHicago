# File author is Ítalo Lima Marconato Matias
#
# Created on November 19 of 2020, at 11:30 BRT
# Last edited on November 28 of 2020, at 13:11 BRT

# This script is supposed to be source'd, not run, as it will setup the path and add some functions to make development easier.

if not set -q ARCH; set ARCH x86; end
if not set -q TYPE; set TYPE Debug; end
if not set -q TOOLCHAIN; set TOOLCHAIN /opt/chicago/toolchain; end
if not set -q SYSROOT; set SYSROOT /opt/chicago/sysroot.$ARCH; end

if not test -d $TOOLCHAIN
	printf "The required toolchain is still not installed, download and extract it to $TOOLCHAIN.\n"
	exit 1
end

# Add the toolchain to the path, and check if everything we need is installed.

set ROOT_DIR (realpath (dirname (status -f))/..)
set PATH $TOOLCHAIN/bin $PATH

if not test -x (command -v clang); printf "The toolchain is incomplete/corrupted (clang is not avaliable).\n"; end
if not test -x (command -v xorriso); printf "The toolchain is incomplete/corrupted (xorriso is not avaliable).\n"; end
if not test -x (command -v mtools); printf "The toolchain is incomplete/corrupted (mtools is not avaliable).\n"; end
#if not test -x (command -v sia-create); printf "The toolchain is incomplete/corrupted (sia-create is not avaliable).\n"; end
if not test -x (command -v cmake); printf "The toolchain is incomplete/corrupted (cmake is not avaliable).\n"; end
if not test -x (command -v ninja); printf "The toolchain is incomplete/corrupted (ninja is not avaliable).\n"; end

# Create the helper functions.

function build
	if not test -d build/$ARCH
		mkdir -p build/$ARCH
		cmake -GNinja -DCMAKE_BUILD_TYPE=$TYPE -DCMAKE_TOOLCHAIN_FILE=$ROOT_DIR/host-tools/cmake/Toolchain/$ARCH.cmake \
		      -DCHOS_ARCH=$ARCH -DCHOS_TOOLCHAIN=$TOOLCHAIN -DCHOS_SYSROOT=$SYSROOT -S . -B build/$ARCH
	end
	
	cmake --build build/$ARCH
end
