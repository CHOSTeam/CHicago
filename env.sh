#!/bin/bash

# Current GCC and Binutils version, only change after updating gcc.patch and binutils.patch (or making sure that they work on the updated version)

BINUTILS_VERSION="2.34"
GCC_VERSION="10.1.0"

# CHicago version (also this shell version)

SIA_VERSION="1.0"
VERSION="next.0"

# Get the root directory of the source code

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Set the destination architecture, where the toolchain is, and some other things

if [ -z ${ARCH+x} ]; then ARCH="x86"; fi								
if [ -z ${EDITOR+x} ]; then EDITOR=nano; fi

if [ -z ${DEBUG+x} ]; then DEBUG="no"; fi

proj=("" "" "" "" "" "" "")
ppath=""

# Now, we need to load some arch-specific files, first, check if the files exist

if [ ! -f $ROOT_DIR/arch/$ARCH/env/toolchain.sh ] ||
   [ ! -f $ROOT_DIR/arch/$ARCH/env/config.sh ] ||
   [ ! -f $ROOT_DIR/arch/$ARCH/env/build.sh ]; then
	printf "Unsupported architecture ($ARCH).\n"
	exit 1
fi

# And load everything

source $ROOT_DIR/arch/$ARCH/env/toolchain.sh
source $ROOT_DIR/arch/$ARCH/env/config.sh
source $ROOT_DIR/arch/$ARCH/env/build.sh

NEWEST=$ROOT_DIR/env.sh

if [ $ROOT_DIR/arch/$ARCH/env/toolchain.sh -nt $NEWEST ]; then
	NEWEST=$ROOT_DIR/arch/$ARCH/env/toolchain.sh
fi

if [ $ROOT_DIR/arch/$ARCH/env/config.sh -nt $NEWEST ]; then
	NEWEST=$ROOT_DIR/arch/$ARCH/env/config.sh
fi

if [ $ROOT_DIR/arch/$ARCH/env/build.sh -nt $NEWEST ]; then
	NEWEST=$ROOT_DIR/arch/$ARCH/env/build.sh
fi

HISTCONTROL=ignoredups:erasedups
HISTFILE="$TOOLCHAIN/chenv_history"
shopt -s histappend

build_binutils() {
	# Building binutils: First, let's create the sysroot and copy the dummy files, create the destination folder,
	# download the source code, extract it, patch it, and call the arch-specific build function (followed by
	# cleaning up everything).
	
	rm -rf $TOOLCHAIN/sysroot
	mkdir -p $TOOLCHAIN/sysroot/{Development,System,Libraries,Programs}
	mkdir -p $TOOLCHAIN/sysroot/Development/{Headers,Libraries,Programs,Sources}
	mkdir -p $TOOLCHAIN/sysroot/System/{Boot,Configurations,Drivers,Fonts,Libraries,Programs,Themes}
	cp -RT $ROOT_DIR/env/dummy/ $TOOLCHAIN/sysroot/Development/Headers/
	
	mkdir -p $TOOLCHAIN/build
	cd $TOOLCHAIN/build
	
	printf "Downloading Binutils source code\n"
	wget -4 -q https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz
	
	printf "Extracting Binutils source code\n"
	tar xpf binutils-$BINUTILS_VERSION.tar.xz
	
	# Let's go into the source folder before patching and calling the arch-specific function
	
	cd binutils-$BINUTILS_VERSION
	
	printf "Patching Binutils source code\n"
	patch --ignore-whitespace -Np1 -s -i $ROOT_DIR/env/binutils.patch
	
	_build_binutils
	
	cd $ROOT_DIR
	rm -rf $TOOLCHAIN/build
	
	# Finally, write the installed version into the 'built' file
	
	if [ ! -f $TOOLCHAIN/built ]; then
		printf "0\n0\n" > $TOOLCHAIN/built
	fi
	
	sed -i "1d" $TOOLCHAIN/built
	sed -i "1i$BINUTILS_VERSION" $TOOLCHAIN/built
}

build_gcc() {
	# Building gcc: Similar to building binutils, but we don't need to create the sysroot.
	
	mkdir -p $TOOLCHAIN/build
	cd $TOOLCHAIN/build
	
	printf "Downloading GCC source code\n"
	wget -4 -q https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz
	
	printf "Extracting GCC source code\n"
	tar xpf gcc-$GCC_VERSION.tar.xz
	
	# Let's go into the source folder before patching and calling the arch-specific function
	
	cd gcc-$GCC_VERSION
	
	printf "Patching GCC source code\n"
	patch --ignore-whitespace -Np1 -s -i $ROOT_DIR/env/gcc.patch
	
	_build_gcc
	
	cd $ROOT_DIR
	rm -rf $TOOLCHAIN/build
	
	# Finally, write the installed version into the 'built' file
	
	if [ ! -f $TOOLCHAIN/built ]; then
		printf "0\n0\n" > $TOOLCHAIN/built
	fi
	
	sed -i "2d" $TOOLCHAIN/built
	echo "$GCC_VERSION" >> $TOOLCHAIN/built
}

build_sia() {
	# Building sia-create: Just go to the source code folder, and compile the .cxx files, setting
	# the destination of the binary to the $TOOCHAIN/bin folder
	
	printf "Building sia-create\n"
	cd $ROOT_DIR/env/sia-create
	g++ -std=gnu++20 -O3 -Wall -Wextra -Wno-implicit-fallthrough \
		-Iinclude \
		-o $TOOLCHAIN/bin/sia-create *.cxx -lstdc++fs
	
	# Finally, write the installed version into the 'built' file
	
	if [ ! -f $TOOLCHAIN/built ]; then
		printf "0\n0\n0\n" > $TOOLCHAIN/built
	fi
	
	sed -i "3d" $TOOLCHAIN/built
	echo "$SIA_VERSION" >> $TOOLCHAIN/built
}

check_toolchain() {
	# First, check if we even built the toolchain before, in case we did, we already know
	# that a file called 'built' will be on the root directory of the toolchain, it contains
	# the installed GCC/Binutils version
	
	if [ ! -f $TOOLCHAIN/built ] || [ ! -e $TOOLCHAIN/bin ]; then
		printf "The build environment detected that there is no toolchain installed, or that the installed toolchain wasn't installed "
		printf "by the recommended method (through this build environment)\n"
		printf "Do you want to build the toolchain into the folder '$TOOLCHAIN'?\n"
		printf "(You can change the toolchain folder by passing TOOLCHAIN=folder to us or by exporting TOOLCHAIN=folder)\n"
		printf "[Y/N] "
		read ans
		printf "\n"
		
		if [ "$ans" != "${ans#[Yy]}" ]; then
			build_binutils
			build_gcc
			build_sia
			build_arch_toolchain
			printf "\n"
		fi
		
		return 
	fi
	
	local nl="0"
	
	# Now, check if the installed version of Binutils is the recommended one
	
	if [ "$BINUTILS_VERSION" != "$(head -n 1 $TOOLCHAIN/built)" ]; then
		printf "The build environment detected that the installed version of Binutils isn't the recommended one.\n"
		printf "Do you want to install the recommended version?\n"
		printf "[Y/N] "
		read ans
		printf "\n"
		
		if [ "$ans" != "${ans#[Yy]}" ]; then
			build_binutils	
			nl="1"
		fi
	fi
	
	# And, check if the installed version of GCC is the recommended one
	
	if [ "$GCC_VERSION" != "$(head -n 2 $TOOLCHAIN/built | tail -n 1)" ]; then
		printf "The build environment detected that the installed version of GCC isn't the recommended one.\n"
		printf "Do you want to install the recommended version?\n"
		printf "[Y/N] "
		read ans
		printf "\n"
		
		if [ "$ans" != "${ans#[Yy]}" ]; then
			build_gcc
			nl="1"
		fi
	fi
	
	# And, check if the installed version of sia-create is the recommended one
	
	if [ "$SIA_VERSION" != "$(head -n 3 $TOOLCHAIN/built | tail -n 1)" ]; then
		printf "The build environment detected that the installed version of sia-create isn't the recommended one.\n"
		printf "Do you want to install the recommended version?\n"
		printf "[Y/N] "
		read ans
		printf "\n"
		
		if [ "$ans" != "${ans#[Yy]}" ]; then
			build_sia
			nl="1"
		fi
	fi
	
	# And check if all the arch-specific programs are on the recommended version
	
	check_arch_toolchain
	
	if [ $? == 1 ]; then
		nl="1"
	fi
	
	if [ "$nl" == "1" ]; then
		printf "\n"
	fi
}

get_proj() {
	declare name=$1
	
	# Check if the project folder and main file exists
	
	if [ ! -d $ROOT_DIR/$name ] || [ ! -f $ROOT_DIR/$name/proj.conf ]; then
		printf "[invalid]"
		return
	fi
	
	# Let's convert the file into an array, and check if the amount of entries is correct (5)
	
	readarray -td, tmp <$ROOT_DIR/$name/proj.conf
	
	if [ ${#tmp[@]} != 5 ]; then
		printf "[invalid]"
		return
	fi
	
	# Return the contents of the proj file
	
	cat $ROOT_DIR/$name/proj.conf
}

write_proj() {
	# First, remove the old proj file
	
	rm $ROOT_DIR/$ppath/proj.conf
	
	# Now, make a new one with all the new content
	
	printf "${proj[0]},${proj[1]},${proj[2]}" > $ROOT_DIR/$ppath/proj.conf
	printf ",${proj[3]},${proj[4]}" >> $ROOT_DIR/$ppath/proj.conf
}

is_code_file() {
	# Currently, we only check for C, C++ and Assembly file extensions
	
	local ext=${1##*.}
	
	if [ "$ext" == "c" ] ||
	   [ "$ext" == "C" ] || [ "$ext" == "cc" ] || [ "$ext" == "cxx" ] || [ "$ext" == "c++" ] ||
	   [ "$ext" == "asm" ] || [ "$ext" == "s" ] || [ "$ext" == "S" ]; then
		return 0
	fi
	
	return 1
}

list_config() {
	printf "TOOLCHAIN\t- Set the toolchain location\n"
	printf "EDITOR\t\t- Set the text editor location\n"
	printf "DEBUG\t\t- Set if the code should be compiled with debug flags on\n"
	printf "ARCH\t\t- Set the build destination architecture\n"
	_list_config
}

get_config() {
	declare name=$1
	
	if [ "$name" == "TOOLCHAIN" ]; then
		printf "$TOOLCHAIN"
	elif [ "$name" == "EDITOR" ]; then
		printf "$EDITOR"
	elif [ "$name" == "DEBUG" ]; then
		printf "$DEBUG"
	elif [ "$name" == "ARCH" ]; then
		printf "$ARCH"
	elif [ "$name" == "SUBARCH" ]; then
		printf "$SUBARCH"
	else
		_get_config $name
	fi
}

set_config() {
	declare name=$1
	declare value=$2
	
	if [ "$name" == "TOOLCHAIN" ] ||
	   [ "$name" == "ARCH" ] ||
	   [ "$name" == "SUBARCH" ]; then
		printf "Can't set '$name' at runtime.\n"
	elif [ "$name" == "EDITOR" ]; then
		EDITOR="$value"
	elif [ "$name" == "DEBUG" ]; then
		DEBUG="$value"
	else
		_set_config $name $value
	fi
}

build() {
	declare name=$1
	
	if [ "$name" == "$ppath" ]; then
		# As you may already think, the current project is already open, so we don't need to manually
		# open it
		
		local bld=("${proj[@]}")
	else
		# We need to manually open the project
		
		local blds=$(get_proj "$name" | tr -d '\n')
		
		# If get_proj returned [invalid], this project doesn't exists, or the proj.conf file is corrupted/missing
		
		if [ "$blds" == "[invalid]" ]; then
			printf "The project '$name' doesn't exists, but is required for the build.\n"
			false
			return
		fi
		
		# If everything is ok, we are ready to go!
		
		readarray -td, bld < <(printf "$blds")
	fi
	
	# First, convert the dependency list (which is currently a string) into an array
	
	if [ "${bld[2]}" == "[none]" ]; then
		local aprojs=()
	else
		readarray -d' ' -t aprojs < <(printf "${bld[2]}")
	fi
	
	if [ "${bld[3]}" == "[none]" ]; then
		local nprojs=()
	else
		readarray -d' ' -t nprojs < <(printf "${bld[3]}")
	fi
	
	# Now, build the dependencies (arch-specific first, followed by all the other ones)
	
	for i in "${aprojs[@]}"; do
		printf "DEP: $i/$ARCH\n"
		
		build "$i/$ARCH"
		res=$?
		
		if [ $res != 0 ]; then
			return $res
		fi
	done
	
	for i in "${nprojs[@]}"; do
		printf "DEP: $i\n"
		
		build "$i"
		res=$?
		
		if [ $res != 0 ]; then
			return $res
		fi
	done
	
	# Get the path where the source files are, where object files are going to be built,
	# and where the final binary is going to be built
	
	local spath=$ROOT_DIR/$name
	local opath=$spath/build/$FULL_ARCH-$([ "$DEBUG" == "yes" ] && printf dbg || printf rel)
	local bname=$spath/build/${bld[0]}-$FULL_ARCH-$([ "$DEBUG" == "yes" ] && printf dbg || printf rel)$(get_ext ${bld[4]})
	
	# Create the required path tree
	
	mkdir -p $opath
	
	# Get the list of source files
	
	local subprojs=($(find $spath -mindepth 2 -name proj.conf -printf '%P\n' | sed 's@/proj.conf@@g'))
	local exclude=""
	
	for i in "${subprojs[@]}"; do
		exclude+="-o -path $spath/$i -prune "
	done
	
	if [[ "$spath" =~ "arch" ]]; then
		local srcs=($(find $spath -path $spath/env -prune \
								  -o -path $spath/subarch -prune \
								  $exclude \
								  -o -name *.c -print \
								  -o -name *.C -print -o -name *.cc -print -o -name *.cxx -print -o -name *.c++ -print \
								  -o -name *.s -print -o -name *.S -print -o -name *.asm -print | \
								  sed -e "s@$spath/@@g"))
	else
		local srcs=($(find $spath -path $spath/arch -prune \
								  -o -path $spath/subarch -prune \
								  $exclude \
								  -o -name *.c -print \
								  -o -name *.C -print -o -name *.cc -print -o -name *.cxx -print -o -name *.c++ -print \
								  -o -name *.s -print -o -name *.S -print -o -name *.asm -print | \
								  sed -e "s@$spath/@@g"))
	fi
	
	# And build file by file
	
	local objs=()
	local rebld=0
	
	for i in "${srcs[@]}"; do
		objs+=("$opath/$i.o")
		
		# Now, check if we really need to build this file
		# If the binary file doesn't exists, we don't need to check anything lol, and if it does,
		# check the modification date
		
		local do=0
		
		if [ ! -e $opath/$i.o ]; then
			do=1
		fi
		
		# Check the date of all the deps
		
		if [ $do == 0 ]; then
			readarray -d' ' -t deps < <(get_deps $spath $i ${bld[4]} | sed -e 's@.*\.o: @@' -e ':b;$!N;/\\\n /s/\\\n //;tb;P;D')
			
			for d in "${deps[@]}"; do
				if [ $d -nt $opath/$i.o ] || [ $NEWEST -nt $opath/$i.o ]; then
					do=1
					break
				fi
			done
			
			if [ $do == 0 ]; then
				continue
			fi
		fi
		
		# Now, just compile it :)
		
		compile $spath $opath $i ${bld[4]}
		res=$?
		
		if [ $res != 0 ]; then
			return $res
		fi
		
		rebld=1
	done
	
	if [ "$name" == "$ppath" ]; then
		local sname="$ROOT_DIR/arch/$ARCH/env/proj/${proj[0]}.sh"
	else
		local sname="$ROOT_DIR/arch/$ARCH/env/proj/${bld[0]}.sh"
	fi
	
	# If a <name>.sh file exists on the env/proj folder of the architecture, run it, as it is probably
	# going to contain more arch-specific/subarch-specific files that aren't on the normal code file list
	
	if [ -f $sname ]; then (
		rebld=0
		
		source $sname
		
		for i in "${ARCH_OBJS[@]}"; do
			printf "$opath/$i.o\n" >> temp.txt
			
			# If the binary file doesn't exists, we don't need to check anything lol, and if it does,
			# check the modification date
			
			local do=0
			
			if [ ! -e $opath/$i.o ]; then
				do=1
			fi
			
			# Check the date of all the deps
			
			if [ $do == 0 ]; then
				readarray -d' ' -t deps < <(get_deps $spath $i ${bld[4]} | sed -e 's@.*\.o: @@' -e ':b;$!N;/\\\n /s/\\\n //;tb;P;D')
				
				for d in "${deps[@]}"; do
					if [ $d -nt $opath/$i.o ] || [ $NEWEST -nt $opath/$i.o ]; then
						do=1
						break
					fi
				done
			fi
			
			if [ $do == 0 ]; then
				continue
			fi
			
			# Now, just compile it :)
			
			compile $spath $opath $i ${bld[4]}
			res=$?
			
			if [ $res != 0 ]; then
				if [ -f temp.txt ]; then
					rm temp.txt
				fi
				
				return ${res#-}
			fi
			
			rebld=1
		done
		
		if [ $rebld == 1 ]; then
			return -1
		else
			return 0
		fi
	) fi
	
	local res=$?
	
	if [ $res -gt 0 ] && [ $res != 255 ]; then
		return $res
	elif [ $res == 255 ]; then
		rebld=1
	fi
	
	if [ -f temp.txt ]; then
		objs+=("$(cat temp.txt)")
		rm temp.txt
	fi
	
	# If the binary doesn't exists, we need to do the linking anyway
	
	if [ ! -e $bname ] &&
	   [ ${#objs[@]} != 0 ]; then
		rebld=1
	fi
	
	if [ "$(force_rebuild ${bld[4]})" == "1" ] &&
	   [ ${#objs[@]} != 0 ]; then
		rebld=1
	fi
	
	# Finally, link the final binary!
	
	if [ $rebld == 1 ]; then
		link ${bld[4]} $bname "${objs[@]}"
		res=$?
		
		if [ $res != 0 ]; then
			return $res
		fi
	fi
	
	# This project may need to execute some arch-specific thing after linking
	
	if [ -f $sname ]; then (
		source $sname
		
		if [ "$(declare -f -F after_build)" != "" ]; then
			after_build
			res=$?
			
			if [ $res != 0 ]; then
				return $res
			fi
		fi
		
		return 0
	) fi
	
	res=$?
	
	# And remove the build folder if nothing was done
	
	if [ -d $opath ] && [ "$(ls -A $opath)" == "" ]; then
		rm -rf $opath
	fi
	
	if [ -d $spath/build ] && [ "$(ls -A $spath/build)" == "" ]; then
		rm -rf $spath/build
	fi
	
	return $res
}

loop_parse() {
	set -f
	local argv=( $1 )
	local argc=${#argv[@]}
	set +f
	
	if [ "${argc}" == "0" ]; then
		return
	else
		history -s "$1"
	fi
	
	if [ "${argv[0]}" == "b" ]; then
		# b: build the project
		# We need to check if we have any open/selected project
		
		if [ "${proj[0]}" == "" ]; then
			printf "There is no open/selected project.\n"
			return
		fi
		
		# And call the main build function
		
		build "$ppath"
	elif [ "${argv[0]}" == "conf" ]; then
		# conf: list/get/set config
		# The amount of arguments say what we gonna do
		
		if [ $argc == 1 ]; then
			list_config
		elif [ $argc == 2 ]; then
			printf "$(get_config ${argv[1]})\n"
		elif [ $argc == 3 ]; then
			set_config ${argv[1]} ${argv[2]}
		else
			printf "Invalid usage of the 'conf' command.\n"
		fi
	elif [ "${argv[0]}" == "cls" ]; then
		# cls: clear the screen
		clear && printf "\e[3J"
	elif [ "${argv[0]}" == "h" ]; then
		# h: help command
		printf "b                   - Build the project\n"
		printf "cls                 - Clear the screen\n"
		printf "conf (name) (value) - List/get/set a configuration (0 args = list, 1 arg = get, 2 args = set)\n"
		printf "h                   - Show all the shell commands\n"
		printf "i                   - Show information about the current open project\n"
		printf "lp                  - List all the projects\n"
		printf "ls (path)           - List files/directory in (path) or in the current directory (path is relative to the selected project path)\n"
		printf "mp [name]           - Create the project [name]\n"
		printf "q                   - Exit the build environment\n"
		printf "rp [name]           - Remove the project [name]\n"
		printf "s [name]            - Select the project [name]\n"
	elif [ "${argv[0]}" == "i" ]; then
		# i: show some info about the open project
		# We need to check if we have any open/selected project
		
		if [ "${proj[0]}" == "" ]; then
			printf "There is no open/selected project.\n"
			return
		fi
		
		# Get all the source code files from the project
		
		local subprojs=($(find $ppath -mindepth 2 -name proj.conf -printf '%P\n' | sed 's@/proj.conf@@g'))
		local exclude=""
		
		for i in "${subprojs[@]}"; do
			exclude+="-o -path $ppath/$i -prune "
		done
		
		if [[ "$ppath" =~ "arch" ]]; then
			local srcs=($(find $ppath -path $ppath/env -prune \
									  -o -path $ppath/subarch -prune \
									  $exclude \
									  -o -name *.c -print \
									  -o -name *.C -print -o -name *.cc -print -o -name *.cxx -print -o -name *.c++ -print \
									  -o -name *.s -print -o -name *.S -print -o -name *.asm -print | \
									  sed -e "s@$ppath/@@g"))
		else
			local srcs=($(find $ppath -path $ppath/arch -prune \
									  -o -path $ppath/subarch -prune \
									  $exclude \
									  -o -name *.c -print \
									  -o -name *.C -print -o -name *.cc -print -o -name *.cxx -print -o -name *.c++ -print \
									  -o -name *.s -print -o -name *.S -print -o -name *.asm -print | \
									  sed -e "s@$ppath/@@g"))
		fi
		
		# If a <name>.sh file exists on the env/proj folder of the architecture, run it, as it is probably
		# going to contain more arch-specific/subarch-specific files that aren't on the normal code file list
		
		if [ -f $ROOT_DIR/arch/$ARCH/env/proj/${proj[0]}.sh ]; then (
			local first=1
			
			source $ROOT_DIR/arch/$ARCH/env/proj/${proj[0]}.sh
			
			for i in "${ARCH_OBJS[@]}"; do
				if [ $first == 0 ]; then
					printf " $i" >> temp.txt
				else
					printf "$i" >> temp.txt
					first=0
				fi
			done
		) fi
		
		if [ -f temp.txt ]; then
			srcs+=("$(cat temp.txt)")
			rm temp.txt
		fi
		
		if [ ${#srcs[@]} == 0 ]; then
			srcs+=("[none]")
		fi
		
		printf "Name                       : '${proj[0]}'\n"
		printf "Description                : '${proj[1]}'\n"
		printf "Arch-Specific Dependencies : '${proj[2]}'\n"
		printf "Regular Dependencies       : '${proj[3]}'\n"
		printf "Project Type               : '${proj[4]}'\n"
		printf "Project Code Files         : '${srcs[*]}'\n"
	elif [ "${argv[0]}" == "lp" ]; then
		# lp: list all the projects
		printf "$(find $ROOT_DIR -name proj.conf -printf '%P\n' | sed 's@/proj.conf@@g')"
		printf "\n"
	elif [ "${argv[0]}" == "ls" ]; then
		# ls: list files/directories
		# We need to check if we have any open/selected project
		
		if [ "${proj[0]}" == "" ]; then
			printf "There is no open/selected project.\n"
			return
		fi
		
		local dir=$ROOT_DIR/$ppath
		
		# Can take (relative to the current project directory) as the first arg
		
		if [ $argc -gt 1 ]; then
			# Check if there isn't any wildcard expression
			
			if [[ "${argv[1]}" =~ "*" ]]; then
				printf "The 'ls' command doesn't support wildcard expressions.\n"
				return
			fi
			
			dir+=/${argv[1]}
		fi
		
		if [ ! -e $dir ]; then
			printf "The specified file/directory (${argv[1]}) doesn't exists.\n"
			return
		fi
		
		ls --color $dir
	elif [ "${argv[0]}" == "mp" ]; then
		# mp: create new project and select it
		# Takes the name of the project as the first arg
		
		if [ $argc != 2 ]; then
			printf "The 'mp' command expects one argument.\n"
			return
		fi
		
		# Check if there isn't any wildcard expression
		
		if [[ "${argv[1]}" =~ "*" ]]; then
			printf "The 'mp' command doesn't support wildcard expressions.\n"
			return
		fi
		
		# Check if the project doesn't already exists
		
		local tmp=$(get_proj ${argv[1]})
		
		if [ "$tmp" != "[invalid]" ]; then
			printf "The specified project (${argv[2]}) already exists.\n"
			return
		fi
		
		mkdir -p $ROOT_DIR/${argv[1]}
		printf "[name],[desc],[arch-deps],[deps],[type],[files]" >$ROOT_DIR/${argv[1]}/proj.conf
		$EDITOR $ROOT_DIR/${argv[1]}/proj.conf
		readarray -td, proj < <(get_proj ${argv[1]} | tr -d '\n')
		ppath="${argv[1]}"
	elif [ "${argv[0]}" == "q" ]; then
		# q: exit the program, returning 0/success (duh)
		exit 0
	elif [ "${argv[0]}" == "rp" ]; then
		# rp: remove project
		
		# Check if the open project isn't the one we're trying to remove
		
		if [ "${proj[0]}" == "${argv[1]}" ]; then
			printf "The project that you're trying to remove is currently open/selected.\n"
			return
		fi
		
		# Now, check if the project exists
		
		local tmp=$(get_proj ${argv[1]} | tr -d '\n')
		
		if [ "$tmp" == "[invalid]" ]; then
			printf "The specified project (${argv[1]}) doesn't exists.\n"
			return
		fi
		
		# And just delete it (TODO: Remove all the references to this project from other projects)
		
		rm -rf $ROOT_DIR/${argv[1]}
	elif [ "${argv[0]}" == "s" ]; then
		# s: change the selected/open project
		# Takes the name of the project as the first arg
		
		if [ $argc != 2 ]; then
			printf "The 's' command expects one argument.\n"
			return
		fi
		
		# Check if there isn't any wildcard expression
		
		if [[ "${argv[1]}" =~ "*" ]]; then
			printf "The 's' command doesn't support wildcard expressions.\n"
			return
		fi
		
		# Check if the project exists
		
		local tmp=$(get_proj ${argv[1]} | tr -d '\n')
		
		if [ "$tmp" == "[invalid]" ]; then
			printf "The specified project (${argv[1]}) doesn't exists.\n"
			return
		fi
		
		# And switch the selected project into it
		
		readarray -td, proj < <(printf "$tmp")
		ppath="${argv[1]}"
	else
		# invalid: just print an error message
		printf "Unrecognized command '${argv[0]}'.\n"
	fi
}

loop_main() {
	while :
	do
		# Read the user input and call the parse method
		
		printf "[Project: ${proj[0]}] [Version: $VERSION] [Arch: $FULL_ARCH] [Debug: $([ "$DEBUG" == "yes" ] && printf yes || printf no)]\n"
		history -n; history -w; history -c; history -r; read -p "> " -e in
		loop_parse "$in"
		printf "\n"
	done
}

# Print some info about this environment (the build environment version DON'T need to match the kernel's version)

printf "CHicago Operating System Build Environment\n"
printf "Version $VERSION\n"
printf "The build environment and the operating system are under development, "
printf "the public API and this very build system may change drastically any time\n\n"

# Check if the toolchain is already compiled and in the latest/recommended version

check_toolchain

# Make sure that we're in the root directory

cd $ROOT_DIR

# Try to get the main project (named chos)

tmp=$(get_proj chos | tr -d '\n')

if [ "$tmp" == "[invalid]" ]; then
	printf "The main project (chos) couldn't be found, starting the shell without any open project\n\n"
else
	readarray -td, proj < <(printf "$tmp")
	ppath="chos"
fi

loop_main
