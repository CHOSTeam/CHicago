# CHicago

CHicago is a portable operating system developed from scratch. The code is in C/C++ (+ a bit of assembly).

# Building

CHicago uses Clang as the compiler, LLD as the linker, CMake as the make generator, and Ninja as the default CMake target. Our Clang/LLD was compiled with an extra target added, and for now the prebuilt toolchain file is not avaliable for download. This will be changed pretty soon (recompiling libgcc and llvm takes way too long on WSL).

If you already installed the toolchain (after it was made public for download), you can source the 'host-tools/env' file (.sh for bash-compatible, .fish for fish) and run 'build' on the root directory, this will create the build directory and invoke CMake with the right arguments.
