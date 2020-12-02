# File author is Ítalo Lima Marconato Matias
#
# Created on November 19 of 2020, at 10:17 BRT
# Last edited on November 29 of 2020, at 15:56 BRT

cmake_minimum_required(VERSION 3.12.0)

# We need to set the module path before calling project(), as we have a custom platform file.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/host-tools/cmake")
project(chos C CXX ASM-ATT)

include(CHOSOptions)

add_subdirectory(boot)
add_subdirectory(kernel)
