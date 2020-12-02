# File author is Ítalo Lima Marconato Matias
#
# Created on November 19 of 2020, at 11:40 BRT
# Last edited on November 28 of 2020, at 12:21 BRT

set(CMAKE_SYSTEM_NAME CHicago)
set(CMAKE_SYSTEM_PROCESSOR x86)

# We don't set the compiler target, as for while most of the projects use the -pc-chicago target, the
# UEFI loader uses the -w32-mingw32 target.

set(CMAKE_ASM-ATT_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_ASM-ATT_FLAGS "-c")
