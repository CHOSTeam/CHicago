## Build instructions

Currently, the whole CHicago kernel, drivers and userspace (the last two were almost inexistent before) are under a rewrite.
The build environment changed from GNU make to a custom build environment made using only bash scripts, and the kernel is now written in C++ instead of C.

You can change the build architecture/sub-architecture by changing the ARCH and SUBARCH environment variable, you can also if the OS is going to be built in debug or "release" mode using the DEBUG environment variable (or by using the 'conf DEBUG [yes/no]' command).

Invoke the build environment by going into the root directory of the repository, and calling the env.sh script (remembering to pass any environemnt variable, if required).
The build environment is going to check if the toolchain is already installed (the path of the toolchain is given using the TOOLCHAIN environment variable), and if it requires any updates.

You can check every command avaliable using the 'h' command, but to start the build you can use the 'b' command.